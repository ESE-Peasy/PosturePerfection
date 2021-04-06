/**
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

#include "pipeline.h"

#include <exception>
#include <string>
#include <utility>

#define MODEL_INPUT_X 224
#define MODEL_INPUT_Y 224
#define CONFIDENCE_THRESH_DEFAULT 0.1

#define ms_to_ns(x) \
  (size_t)(x *      \
           (size_t)1000000)  ///< Convert value in milliseconds to nanoseconds

namespace Pipeline {

FrameGenerator::FrameGenerator(void) : cap(0) {
  if (!cap.isOpened()) {
    throw std::runtime_error("Cannot access camera");
  }
  // Ensure there is always a frame ready
  cv::Mat frame;
  cap.read(frame);
  if (frame.empty()) {
    fprintf(stderr, "Empty frame from camera\n");
    return;
  }
  current_frame = frame;

  // Start thread that continuously gets newest frame
  std::thread t(&FrameGenerator::FrameGenerator::thread_body, this);
  thread = std::move(t);
}

FrameGenerator::~FrameGenerator() {
  running = false;
  thread.join();
}

void FrameGenerator::updated_framerate(size_t new_frame_delay) {
  stop();
  start(ms_to_ns(new_frame_delay));
}

void FrameGenerator::timerEvent(void) { cv.notify_one(); }

void FrameGenerator::thread_body(void) {
  while (running) {
    cv::Mat frame;
    cap.read(frame);

    if (frame.empty()) {
      fprintf(stderr, "Empty frame\n");
      return;
    }

    std::unique_lock<std::mutex> lock(mutex);
    current_frame = frame;
    lock.unlock();
  }
}

RawFrame FrameGenerator::next_frame(void) {
  // Lock so only a single thread can get next frame at once
  std::unique_lock<std::mutex> lock(mutex);
  cv.wait(lock);
  auto output = RawFrame{id++, current_frame};
  lock.unlock();
  return output;
}

void Pipeline::core_thread_body(Inference::InferenceCore core) {
  while (running) {
    auto raw_next_frame = frame_generator.next_frame();
    auto preprocessed_image = preprocessor.run(raw_next_frame.raw_image);

    auto core_result = core.run(preprocessed_image);

    core_results.push(CoreResults{
        raw_next_frame.id, std::move(raw_next_frame.raw_image), core_result});
  }
}

void Pipeline::post_processing_thread_body() {
  while (running) {
    auto next_frame = core_results.pop();
    if (!next_frame.valid) {
      // Buffer has been told to stop
      break;
    }

    auto pose_result = posture_estimator.runEstimator(
        post_processor.run(next_frame.value.image_results));

    posture_estimator.analysePosture(pose_result, next_frame.value.raw_image);

    callback(pose_result, next_frame.value.raw_image);
  }
}

Pipeline::Pipeline(uint8_t num_inference_core_threads,
                   void (*callback)(PostureEstimating::PoseStatus, cv::Mat))
    : framerate_settings(this),
      preprocessor(MODEL_INPUT_X, MODEL_INPUT_Y),
      // Disable smoothing with empty settings
      post_processor(
          CONFIDENCE_THRESH_DEFAULT,
          framerate_settings.get_framerate_setting().smoothing_settings),
      posture_estimator(),
      frame_generator(),
      core_results(&this->running, num_inference_core_threads),
      callback(callback) {
  if (num_inference_core_threads == 0) {
    throw std::invalid_argument("num_inference_core_threads must not be zero");
  }
  this->running = true;

  // Create multiple inference core threads to improve performance
  for (; num_inference_core_threads > 0; num_inference_core_threads--) {
    Inference::InferenceCore core("models/EfficientPoseRT_LITE.tflite",
                                  MODEL_INPUT_X, MODEL_INPUT_Y);

    std::thread core_thread(&Pipeline::Pipeline::core_thread_body, this,
                            std::move(core));
    threads.push_back(std::move(core_thread));
  }

  std::thread post_processing_thread(
      &Pipeline::Pipeline::post_processing_thread_body, this);
  threads.push_back(std::move(post_processing_thread));

  frame_generator.start(
      ms_to_ns(framerate_settings.get_framerate_setting().frame_delay));
}

Pipeline::~Pipeline() {
  printf("Pipeline stopping\n");
  this->running = false;
  for (auto& t : this->threads) {
    t.join();
  }
}

bool Pipeline::set_confidence_threshold(float threshold) {
  return post_processor.set_confidence_threshold(threshold);
}

float Pipeline::get_confidence_threshold() {
  return post_processor.get_confidence_threshold();
}

void Pipeline::updated_framerate(FramerateSetting new_settings) {
  frame_generator.updated_framerate(new_settings.frame_delay);
  post_processor = PostProcessing::PostProcessor(
      get_confidence_threshold(), new_settings.smoothing_settings);
}

float Pipeline::increase_framerate(void) {
  framerate_settings.increase_framerate();
  return get_framerate();
}

float Pipeline::decrease_framerate(void) {
  framerate_settings.decrease_framerate();
  return get_framerate();
}

float Pipeline::get_framerate(void) {
  return 1000.0 / framerate_settings.get_framerate_setting().frame_delay;
}

void Pipeline::set_ideal_posture(PostureEstimating::Pose pose) {
  posture_estimator.update_ideal_pose(pose);
}

bool Pipeline::set_pose_change_threshold(float threshold) {
  return posture_estimator.set_pose_change_threshold(threshold);
}

float Pipeline::get_pose_change_threshold() {
  return posture_estimator.get_pose_change_threshold();
}

}  // namespace Pipeline
