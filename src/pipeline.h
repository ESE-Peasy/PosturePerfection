/**
 * @file pipeline.h
 * @brief Main pipeline for the PosturePerfection system
 *
 * @copyright Copyright (C) 2021  Miklas Riechmann
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef SRC_PIPELINE_H_
#define SRC_PIPELINE_H_

#include <deque>
#include <mutex>   //NOLINT [build/c++11]
#include <thread>  //NOLINT [build/c++11]
#include <vector>

#include "iir.h"
#include "inference_core.h"
#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp"
#include "post_processor.h"
#include "posture_estimator.h"
#include "pre_processor.h"

#define FRAME_DELAY_MAX 2000      ///< Maximum settable frame delay, i.e., 0.5Hz
#define FRAME_DELAY_MIN 50        ///< Minimum settable frame delay, i.e., 20Hz
#define FRAME_DELAY_DEFAULT 1000  ///< Default delay between frames in ms

/**
 * @brief A synchronising buffer and results structure
 *
 */
namespace Buffer {

/**
 * @brief Result when calling `Buffer::pop()`
 *
 * The structure has a field to indicate the validity of the result. It is
 * possible for the buffer to return a result when the owning pipeline is shut
 * down. In this situation the result could be invalid as the call to `pop()`
 * may be blocking until data is available. In this situation a `valid` flag of
 * `false` means the `value` field should not be used. A `valid` flag of `true`
 * means the `value` is useable from the buffer's perspective.
 *
 * @tparam T Type of elements on the buffer
 */
template <typename T>
struct PopResult {
  T value;     ///< Actual popped result
  bool valid;  ///< Indicates validity of the result
};

/**
 * @brief A synchronising buffer to be used as a communication mechanism between
 * threads
 *
 * The buffer ensures that elements are in order based on the element's `id`
 * field. Attempting to `push` an element that is not next in line will block
 * until the missing element has been pushed by a different thread. An object of
 * this class may therefore be used to share the load at any stage in a pipeline
 * between multiple threads without needing to worry about issues in the order
 * of elements.
 *
 * It is implemented as a circular buffer.
 *
 * @tparam T The type for elements in the buffer. This must provide an `id`
 * field that is a `uint8_t`.
 */
template <typename T>
class Buffer {
 private:
  std::mutex lock_in;    ///< `std::mutex` for input to the object
  std::mutex lock_out;   ///< `std::mutex` for output of the object
  std::vector<T> queue;  ///< Underlying queueing mechanism

  size_t front_index = 0;  ///< Front (next to be popped) of the buffer
  size_t back_index = 0;   ///< Back (where to push) of the buffer

  /**
   * @brief Flag to indicate if the pipeline is running
   *
   * When this becomes `false` any blocked threads should be allowed to quit,
   * i.e., infinite loops should exit
   *
   */
  bool* running;

  /**
   * @brief ID number for the newest frame on the queue
   *
   * The next frame that will be added to the queue will therefore be `id + 1`.
   * An ID of `255` is defined to be followed by an ID of `0`, meaning there are
   * no problems due to an integer overflow.
   *
   */
  uint8_t id = -1;

  /**
   * @brief Get the current number of elements in the buffer
   *
   * @return `size_t` number of elements in the buffer
   */
  size_t size(void) {
    int size = back_index - front_index;
    if (size < 0) {
      return (queue.size() + size);
    }
    return size;
  }

  /**
   * @brief Flag to indicate if the buffer is full
   *
   */
  bool full = false;

 public:
  /**
   * @brief Construct a new `Buffer` object
   *
   * @param running_ptr Pointer to the `Pipeline::Pipeline::running` flag
   * @param max_size Maximum desired useable size of underlying memory
   */
  explicit Buffer(bool* running_ptr, size_t max_size)
      : running(running_ptr), queue(max_size) {}

  /**
   * @brief Push a frame to the queue
   *
   * This blocks if the frame's ID is not the next in the series, until the
   * missing frame has been pushed by another thread. Also blocks if the maximum
   * size is reached, until elements are popped.
   *
   * @tparam frame Frame to be pushed to the queue
   */
  void push(T frame) {
    while (*running) {
      lock_in.lock();
      if ((uint8_t)(id + 1) == frame.id && !full) {
        queue.at(back_index) = frame;
        back_index = (back_index + 1) % queue.size();
        if (back_index == front_index) {
          full = true;
        }

        id++;
        lock_in.unlock();
        break;
      }
      lock_in.unlock();
    }
  }

  /**
   * @brief Push a frame to the queue (doesn't block if queue is full)
   *
   * This blocks if the frame's ID is not the next in the series, until the
   * missing frame has been pushed by another thread. It returns if the queue is
   * full.
   *
   * @param frame Frame to be pushed to the queue
   * @return `true` If pushing was successful
   * @return `false` If the frame was not pushed, i.e., the queue was full; or
   * the pipeline is halting
   */
  bool try_push(T frame) {
    while (*running) {
      // Immediately return if the queue is full
      lock_in.lock();
      if (full) {
        lock_in.unlock();
        return false;
      }

      if ((uint8_t)(id + 1) == frame.id) {
        queue.at(back_index) = frame;
        back_index = (back_index + 1) % queue.size();
        if (front_index == back_index) {
          full = true;
        }

        id++;
        lock_in.unlock();
        return true;
      } else {
        lock_in.unlock();
      }
    }
    return false;
  }

  /**
   * @brief Pop the oldest element in the queue and return it
   *
   * @return `PopResult<T>` Oldest frame on the queue. Note: the `valid` flag of
   * the result must be checked before use.
   */
  PopResult<T> pop() {
    PopResult<T> front = PopResult<T>{T{}, false};
    while (*running) {
      this->lock_out.lock();
      if (size() != 0 || full) {
        front = PopResult<T>{this->queue.at(front_index), true};
        front_index = (front_index + 1) % queue.size();
        full = false;

        this->lock_out.unlock();
        break;
      }
      this->lock_out.unlock();
    }
    return front;
  }
};
}  // namespace Buffer

/**
 * @brief Components of the pipeline at the core of the system
 *
 * To use the pipeline one simply needs to initialise a `Pipeline::Pipeline` and
 * it will start to run.
 *
 */
namespace Pipeline {

/**
 * @brief Parameters relevant to the currently set frame rate
 *
 */
struct FramerateSetting {
  IIR::SmoothingSettings
      smoothing_settings;  ///< Smoothing settings (coefficients) for the IIR
                           ///< filter
  size_t frame_delay;  ///< Delay in ms that represents the current frame rate
};

// Forward declaration of `Pipeline` for use in `FramerateSettings`
class Pipeline;

/**
 * @brief Class to maintain the currently set frame rate and any related
 * settings
 */
class FramerateSettings {
 private:
  /**
   * @brief Structure containing presets for various frame rates
   *
   * Check `framerate_settings.cpp` for the possible presets and the default, in
   * order of ascending frame rate
   *
   */
  std::vector<FramerateSetting> framerate_settings;

  /**
   * @brief Index indicating the current setting
   *
   */
  size_t current_setting;

  /**
   * @brief Pointer to the `Pipeline`
   *
   * Only `Pipeline::updated_framerate` is used as a callback to notify the
   * pipeline of an updated frame rate.
   *
   */
  Pipeline* pipeline;

  /**
   * @brief Internal function that is called to notify relevant objects of a
   * change in frame rate
   *
   */
  void notify_pipeline(void);

 public:
  /**
   * @brief Construct a new `FramerateSettings` object
   *
   * @param pipeline `Pipeline*` pointer to the pipeline
   */
  explicit FramerateSettings(Pipeline* pipeline);

  /**
   * @brief Get the currently set `FramerateSetting`
   *
   * @return `FramerateSetting`
   */
  FramerateSetting get_framerate_setting(void);

  /**
   * @brief Increase the frame rate
   *
   */
  void increase_framerate(void);

  /**
   * @brief Decrease the frame rate
   *
   */
  void decrease_framerate(void);
};

/**
 * @brief Contains the id of the frame within the pipeline, as well as the raw
 * image and the preprocessed_image. A struct of this type is created for every
 * input image after it has been passed through the
 * `PreProcessing::PreProcessor`, before being sent to the
 * `Inference::InferenceCore`
 */
struct PreprocessedFrame {
  /**
   * @brief Every frame is assigned a unique id to ensure in-order post
   * processing. This id must be passed through the entire pipeline.
   *
   */
  uint8_t id;
  cv::Mat raw_image;
  PreProcessing::PreProcessedImage preprocessed_image;
};

/**
 * @brief A frame as returned by the `FrameGenerator`
 *
 */
struct RawFrame {
  uint8_t id;         ///< Frame ordering ID
  cv::Mat raw_image;  ///< Raw `cv::Mat` (OpenCV) image
};

class FrameGenerator {
 private:
  /**
   * @brief Video stream handle
   *
   */
  cv::VideoCapture cap;

  /**
   * @brief Identifier to keep track of frame ordering
   *
   * Each frame is labelled with an ID that increments by one from one frame to
   * the next. The FrameGenerator never skips and ID. Once the maximum value is
   * reached, the number overflows and wraps back to zero. Therefore the maximum
   * number possible for the ID is less than zero, i.e., it is always possible
   * to determine what the next expected frame ID is.
   *
   * Access to this should be protected by `lock`
   *
   */
  uint8_t id = 0;

  /**
   * @brief The most up-to-date frame retrieved from the camera
   *
   * Access to this should be protected by `lock`
   *
   */
  cv::Mat current_frame;

  /**
   * @brief Lock to protect the most current frame-related data
   *
   * Enables the synchronisation of `id` and `current_frame`
   *
   */
  std::mutex lock;

  /**
   * @brief Lock to ensure only a single thread can retrieve a frame at once
   *
   */
  std::mutex lock_out;

  /**
   * @brief The thread that retrieves the newest frame from the camera
   *
   */
  std::thread thread;

  /**
   * @brief Time at which the previous capture took place
   *
   * A new capture should only take place after `frame_delay` time has passed
   * since this `t_previous_capture`
   *
   */
  std::chrono::time_point<std::chrono::steady_clock> t_previous_capture;

  /**
   * @brief Currently set delay between frames
   *
   */
  size_t frame_delay;

  /**
   * @brief Implementation of how to retrieve the next frame from the camera
   *
   */
  void thread_body(void);

  /**
   * @brief Flag to tell `thread_body` whether or not it should be running
   *
   */
  bool running = true;

 public:
  void updated_framerate(size_t new_frame_delay);
  /**
   * @brief Construct a new Frame Generator object
   *
   * @param framerate_settings Pointer to the `Pipeline`'s `FramerateSettings`
   * @throw `std::runtime_error` if the camera cannot be accessed
   */
  explicit FrameGenerator(FramerateSettings* framerate_settings);

  /**
   * @brief Destroy the Frame Generator object
   *
   * Blocks until the `thread` that generates the frames is joined
   *
   */
  ~FrameGenerator();

  /**
   * @brief Get the newest frame
   *
   * @return `RawFrame` The most up-to-date frame from the camera
   */
  RawFrame next_frame(void);
};

/**
 * @brief Contains the id of the frame within the pipeline, as well as the raw
 * image and the results of running inference on that image. A struct of this
 * type is created for every input image after it has been passed through the
 * `InferenceCore`, before being sent to the `PostProcessor`
 */
struct CoreResults {
  /**
   * @brief Every frame is assigned a unique id to ensure in-order post
   * processing. This id must be passed through the entire pipeline.
   *
   */
  uint8_t id;
  cv::Mat raw_image;
  Inference::InferenceResults image_results;
};

/**
 * @brief Frame-by-frame pipeline to process video
 *
 * The pipeline for performing all frame-by-frame processing steps, including
 * the pose estimation model. The pipeline incorporates the video capturing
 * stage. The pipeline generates output in the form of a callback, which is
 * called once per frame that passes through the pipeline.
 *
 */
class Pipeline {
 private:
  /**
   * @brief Vector of all threads created in the pipeline
   *
   * The de-constructor iterates through this and calls `join()` an each thread
   * to ensure a clean exit.
   *
   */
  std::vector<std::thread> threads;

  /**
   * @brief Flag to indicate the pipeline is running
   *
   * Setting this to `false` causes threads to terminate
   *
   */
  bool running;

  FramerateSettings framerate_settings;

  PreProcessing::PreProcessor preprocessor;
  PostProcessing::PostProcessor post_processor;
  PostureEstimating::PostureEstimator posture_estimator;

  FrameGenerator frame_generator;
  Buffer::Buffer<CoreResults> core_results;

  /**
   * @brief Function that provides the body for the inference core thread
   *
   */
  void core_thread_body(Inference::InferenceCore);

  /**
   * @brief Function that provides the body for the post processing thread
   *
   */
  void post_processing_thread_body(void);

  /**
   * @brief A callback handler called for every frame coming out of the pipeline
   *
   * This function is called once for every frame that passes out of the
   * pipeline and provides a `PostureEstimating::PoseStatus` structure as well
   * as the original image that led to these results. It is then up to the
   * callback to handle the output. As this is a callback, the body of the
   * function should not include compute-heavy code as this will slow down the
   * output frame rate.
   *
   */
  void (*callback)(PostureEstimating::PoseStatus, cv::Mat);

  /**
   * @brief Function to overlay a stick figure of user posture over the input
   * image
   *
   * @param `PostureEstimating::PoseStatus` The pose status output for the
   * current frame
   * @param `cv::Mat` The raw image for the current frame
   * @param `float` The angle threshold (in radians) above which a posture is
   * determined to be bad
   */
  void overlay_image(PostureEstimating::PoseStatus pose_status,
                     cv::Mat raw_image, float pose_change_threshold);

 public:
  void updated_framerate(FramerateSetting new_settings);

  /**
   * @brief Construct a new Pipeline object
   *
   * The pipeline starts upon construction and starts producing output
   *
   * @param num_inference_core_threads The number of threads to use for the
   * inference core stage
   * @param callback Function to call for every frame output by the pipeline
   */
  explicit Pipeline(uint8_t num_inference_core_threads,
                    void (*callback)(PostureEstimating::PoseStatus, cv::Mat));

  /**
   * @brief Destroy the Pipeline object
   *
   * Stops threads and waits for them to complete
   *
   */
  ~Pipeline();

  /**
   * @brief Set the confidence threshold
   *
   * @param threshold New threshold to set
   * @return `true` If updating the threshold succeeded
   * @return `false` If updating the threshold did not succeed
   */
  bool set_confidence_threshold(float threshold);

  /**
   * @brief Get the confidence threshold object
   *
   * @return `float` Currently set confidence threshold
   */
  float get_confidence_threshold();

  /**
   * @brief Increment the frame rate to the next predefined value
   *
   * If the maximum frame rate of the system is reached, it will not be
   * increased further and the current (unchanged) frame rate is returned.
   *
   * @return float Currently set frame rate in Hz
   */
  float increase_framerate(void);

  /**
   * @brief Decrement the frame rate to the next predefined value
   *
   * If the minimum frame rate of the system is reached, it will not be
   * decreased further and the current (unchanged) frame rate is returned.
   *
   * @return float Currently set frame rate in Hz
   */
  float decrease_framerate(void);

  /**
   * @brief Get the frame rate
   *
   * @return float currently set frame rate in Hz
   */
  float get_framerate(void);

  /**
   * @brief Set the ideal posture
   *
   * @param pose A previous frame's posture that is to be used as the new
   * ideal posture
   */
  void set_ideal_posture(PostureEstimating::Pose pose);

  /**
   * @brief Set the pose change threshold
   *
   * @param threshold New threshold to set
   * @return `true` If updating the threshold succeeded
   * @return `false` If updating the threshold did not succeed
   */
  bool set_pose_change_threshold(float threshold);

  /**
   * @brief Get the current pose change threshold
   *
   * @return `float` Currently set pose change threshold
   */
  float get_pose_change_threshold();
};
}  // namespace Pipeline
#endif  // SRC_PIPELINE_H_
