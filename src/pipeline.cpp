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

namespace Pipeline {

FrameGenerator::FrameGenerator(size_t* frame_delay)
    : frame_delay(frame_delay), cap(0) {
  t_previous_capture = std::chrono::steady_clock::now();
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

void FrameGenerator::thread_body(void) {
  while (running) {
    cv::Mat frame;
    cap.read(frame);

    if (frame.empty()) {
      fprintf(stderr, "Empty frame\n");
      return;
    }

    lock.lock();
    current_frame = frame;
    lock.unlock();
  }
}

RawFrame FrameGenerator::next_frame(void) {
  // Lock so only a single thread can get next frame at once
  lock_out.lock();

  // Block until frame delay has elapsed
  std::chrono::duration<double, std::milli> t_since_last_request =
      std::chrono::steady_clock::now() - t_previous_capture;
  while (t_since_last_request <= std::chrono::milliseconds(*frame_delay)) {
    t_since_last_request =
        std::chrono::steady_clock::now() - t_previous_capture;
  }
  t_previous_capture = std::chrono::steady_clock::now();

  lock.lock();
  auto output = RawFrame{id++, current_frame};
  lock.unlock();
  lock_out.unlock();
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

    overlay_image(pose_result, next_frame.value.raw_image);

    callback(pose_result, next_frame.value.raw_image);
  }
}

void Pipeline::overlay_image(PostureEstimating::PoseStatus pose_status,
                             cv::Mat raw_image) {
  PostureEstimating::Pose current = pose_status.current_pose;

  int imageWidth = raw_image.cols;
  int imageHeight = raw_image.rows;

  cv::cvtColor(raw_image, raw_image, cv::COLOR_BGR2RGB);

  for (int i = JointMin + 1; i <= JointMax - 2; i++) {
    if (current.joints[i].coord.status == PostProcessing::Trustworthy &&
        current.joints[i - 1].coord.status == PostProcessing::Trustworthy) {
      cv::Point upper(
          static_cast<int>(current.joints[i - 1].coord.x * imageWidth),
          static_cast<int>(current.joints[i - 1].coord.y * imageHeight));

      cv::Point curr(static_cast<int>(current.joints[i].coord.x * imageWidth),
                     static_cast<int>(current.joints[i].coord.y * imageHeight));
      cv::line(raw_image, upper, curr, colours.at(i), 5);
    }
  }
}

Pipeline::Pipeline(uint8_t num_inference_core_threads,
                   void (*callback)(PostureEstimating::PoseStatus, cv::Mat))
    : preprocessor(MODEL_INPUT_X, MODEL_INPUT_Y),
      // Disable smoothing with empty settings
      post_processor(
          0.1, IIR::SmoothingSettings{std::vector<std::vector<float>>{
                   {{0.00289819, 0.00579639, 0.00289819, 1., -0.72654253, 0.},
                    {1., 1., 0., 1., -1.64755222, 0.73233892}}}}),
      posture_estimator(),
      frame_generator(&frame_delay),
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

float Pipeline::increase_framerate(void) {
  // Increasing frame rate means decreasing frame delay
  auto new_frame_delay = frame_delay >> 1;
  if (new_frame_delay >= FRAME_DELAY_MIN) {
    // Won't be too low, so can update
    frame_delay = new_frame_delay;
  }
  return 1000.0 / frame_delay;
}

float Pipeline::decrease_framerate(void) {
  // Decreasing frame rate means decreasing frame delay
  auto new_frame_delay = frame_delay << 1;
  if (new_frame_delay <= FRAME_DELAY_MAX) {
    // Won't be too high, so can update
    frame_delay = new_frame_delay;
  }
  return 1000.0 / frame_delay;
}

float Pipeline::get_framerate(void) { return 1000.0 / frame_delay; }

void Pipeline::set_ideal_posture(PostureEstimating::Pose pose) {
  posture_estimator.update_ideal_pose(pose);
}

}  // namespace Pipeline
