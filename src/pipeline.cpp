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

#include <string>
#include <utility>

#define MODEL_INPUT_X 224
#define MODEL_INPUT_Y 224

namespace Pipeline {

void Pipeline::input_thread_body() {
}

FrameGenerator::FrameGenerator(size_t* frame_delay)
    : frame_delay(frame_delay), cap(0) {
  printf("FrameGenerator()\n");
  t_previous_capture = std::chrono::steady_clock::now();

  if (!cap.isOpened()) {
    fprintf(stderr, "Can't access camera\n");
    return;
  }

  std::thread t(&FrameGenerator::FrameGenerator::thread_body, this);

  thread = std::move(t);
  lock_out.lock();
}

FrameGenerator::~FrameGenerator() { thread.join(); }

void FrameGenerator::thread_body(void) {
  printf("inside thread\n");
  {
    cv::Mat frame;
    cap.read(frame);
    // printf("read fraem\n");

    if (frame.empty()) {
      fprintf(stderr, "Empty frame\n");
      return;
    }

    lock.lock();
    current_frame = frame;
    lock.unlock();
  }
  lock_out.unlock();
  while (true) {
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
  printf("next_frame()\n");
  lock_out.lock();
  std::chrono::duration<double, std::milli> t_since_last_request =
      std::chrono::steady_clock::now() - t_previous_capture;

  while (t_since_last_request <= std::chrono::milliseconds(*frame_delay)) {
    t_since_last_request =
        std::chrono::steady_clock::now() - t_previous_capture;
  }
  t_previous_capture = std::chrono::steady_clock::now();

  printf("about to lock in next_frame()\n");
  std::fflush(stdout);

  lock.lock();
  printf("got lock in next_frame()\n");
  std::fflush(stdout);

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

    core_results.push(
        CoreResults{raw_next_frame.id, raw_next_frame.raw_image, core_result});
  }
}

void Pipeline::post_processing_thread_body() {
  while (running) {

    auto next_frame = core_results.pop();

    auto pose_result = posture_estimator.runEstimator(
        post_processor.run(next_frame.image_results));

    // Put the `frame.id` on the image for debugging purposes
    auto str = std::to_string(next_frame.id);
    cv::putText(next_frame.raw_image, str.c_str(), cv::Point(20, 20),
                cv::FONT_HERSHEY_DUPLEX, 1.0, cv::Scalar(255, 0, 0));

    callback(pose_result, next_frame.raw_image);
  }
}

Pipeline::Pipeline(uint8_t num_inference_core_threads,
                   void (*callback)(PostureEstimating::PoseStatus, cv::Mat))
    : preprocessor(MODEL_INPUT_X, MODEL_INPUT_Y),
      // Disable smoothing with empty settings
      post_processor(0.1,
                     IIR::SmoothingSettings{std::vector<std::vector<float>>{}}),
      posture_estimator(),
      // preprocessed_frames(&this->running, num_inference_core_threads),
      frame_generator(&frame_delay),
      core_results(&this->running, num_inference_core_threads),
      callback(callback) {
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

  // Start capturing frames after everything is set up
  std::thread input_thread(&Pipeline::Pipeline::input_thread_body, this);
  threads.push_back(std::move(input_thread));
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

float Pipeline::increase_framerate(void) {
  // Increasing frame rate means decreasing frame delay
  auto new_frame_delay = frame_delay << 1;
  if (new_frame_delay >= FRAME_DELAY_MIN) {
    // Won't be too low, so can update
    frame_delay = new_frame_delay;
  }
  return 1.0 / frame_delay;
}

float Pipeline::decrease_framerate(void) {
  // Decreasing frame rate means decreasing frame delay
  auto new_frame_delay = frame_delay >> 1;
  if (new_frame_delay <= FRAME_DELAY_MAX) {
    // Won't be too high, so can update
    frame_delay = new_frame_delay;
  }
  return 1.0 / frame_delay;
}

float Pipeline::get_framerate(void) { return 1.0 / frame_delay; }

void Pipeline::set_ideal_posture(PostProcessing::ProcessedResults posture) {
  posture_estimator.update_ideal_pose(posture);
}

}  // namespace Pipeline
