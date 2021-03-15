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
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "post_processor.h"
#include "posture_estimator.h"
#include "pre_processor.h"

/**
 * @brief Components of the pipeline at the core of the system
 *
 * To use the pipeline one simply needs to initialise a `Pipeline::Pipeline` and
 * it will start to run.
 *
 */
namespace Pipeline {

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
 * @tparam T The type for elements in the buffer. This must provide an `id`
 * field that is a `uint8_t`.
 */
template <typename T>
class Buffer {
 private:
  /**
   * @brief `std::mutex` for input to the object
   *
   */
  std::mutex lock_in;

  /**
   * @brief `std::mutex` for output of the object
   *
   */
  std::mutex lock_out;

  /**
   * @brief Underlying queueing mechanism
   *
   */
  std::deque<T> queue;

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
   * @brief The maximum size of the queue to build up
   *
   * The queue's size cannot be expanded past this limit and this is enforced by
   * the push methods.
   *
   */
  size_t max_size;

 public:
  /**
   * @brief Construct a new Buffer object
   *
   * @param running_ptr Pointer to the `Pipeline::Pipeline::running` flag
   */
  explicit Buffer(bool* running_ptr, size_t max_size)
      : running(running_ptr), max_size(max_size) {}

  /**
   * @brief Push a frame to the queue
   *
   * This blocks if the frame's ID is not the next in the series, until the
   * missing frame has been pushed by another thread. Also blocks if the maximum
   * size is reached, until elements are popped.
   *
   * @param frame Frame to be pushed to the queue
   */
  void push(T frame) {
    while (*running) {
      lock_in.lock();
      if ((uint8_t)(id + 1) == frame.id && queue.size() + 1 <= max_size) {
        queue.push_back(frame);
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
   * @return true If pushing was successful
   * @return false If the frame was not pushed, i.e., the queue was full; or the
   * pipeline is halting
   */
  bool try_push(T frame) {
    while (*running) {
      // Immediately return if the queue is full
      lock_in.lock();
      if (queue.size() + 1 > max_size) {
        lock_in.unlock();
        return false;
      }

      if ((uint8_t)(id + 1) == frame.id) {
        queue.push_back(frame);
        id++;
        lock_in.unlock();
        return true;
      }
      lock_in.unlock();
    }
    return false;
  }

  /**
   * @brief Pop the oldest element in the queue and return it
   *
   * @return T Oldest frame on the queue
   */
  T pop() {
    T front;
    while (*running) {
      this->lock_out.lock();
      if (!this->queue.empty()) {
        front = this->queue.front();
        this->queue.pop_front();
        this->lock_out.unlock();
        break;
      }
      this->lock_out.unlock();
    }
    return front;
  }
};

struct PreprocessedFrame {
  uint8_t id;
  cv::Mat raw_image;
  PreProcessing::PreProcessedImage preprocessed_image;
};

struct CoreResults {
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

  PreProcessing::PreProcessor preprocessor;
  PostProcessing::PostProcessor post_processor;
  PostureEstimating::PostureEstimator posture_estimator;

  Buffer<PreprocessedFrame> preprocessed_frames;
  Buffer<CoreResults> core_results;

  /**
   * @brief Function that provides the body for the input thread
   *
   */
  void input_thread_body(void);

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

 public:
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
};
}  // namespace Pipeline
#endif  // SRC_PIPELINE_H_
