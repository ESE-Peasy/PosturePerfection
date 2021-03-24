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
  cv::VideoCapture cap(0);
  if (!cap.isOpened()) {
    fprintf(stderr, "Can't access camera\n");
    return;
  }

  uint8_t id = 0;

  while (running) {
    auto prev_time = std::chrono::steady_clock::now();
    cv::Mat frame;
    cap.read(frame);
    if (frame.empty()) {
      fprintf(stderr, "Empty frame\n");
      return;
    }
    printf("new frame %d\n", id);
    std::fflush(stdout);

    if (preprocessed_frames.try_push(
            PreprocessedFrame{id, frame, preprocessor.run(frame)})) {
      printf("preprocessed and pushed frame %d\n", id);
      std::fflush(stdout);

      std::chrono::duration<double, std::milli> frame_age =
          std::chrono::steady_clock::now() - prev_time;

      std::cout << "Frame " << std::to_string(id) << " captured "
                << frame_age.count() << "ms ago\n"
                << std::flush;

      id++;
      // cv::imshow("", frame);
      // if (cv::waitKey(5) >= 0) {
      //   running = false;
      // }
      // Set the time to something appropriate for the system. This dictates the
      // frame-rate at which the system operates.
      std::this_thread::sleep_for(std::chrono::milliseconds(frame_delay));
    }
  }
}

void Pipeline::core_thread_body(Inference::InferenceCore core) {
  while (running) {
    printf("core thread attempt to pop\n");
    std::fflush(stdout);

    // preprocessed_frames.notify_thread_ready();
    auto next_frame = preprocessed_frames.pop();
    cv::imshow("", next_frame.raw_image);
      if (cv::waitKey(5) >= 0) {
        running = false;
      }

    printf("core thread run %d\n", next_frame.id);
    std::fflush(stdout);

    auto core_result = core.run(next_frame.preprocessed_image);
    std::this_thread::sleep_for(std::chrono::milliseconds(frame_delay*2));

    printf("core thread pushing %d\n", next_frame.id);
    std::fflush(stdout);

    core_results.push(
        CoreResults{next_frame.id, next_frame.raw_image, core_result});
  }
}

void Pipeline::post_processing_thread_body() {
  while (running) {
    // printf("post proc. pop\n");
    // std::fflush(stdout);

    auto next_frame = core_results.pop();

    // printf("post proc. run %d\n", next_frame.id);
    // std::fflush(stdout);

    auto pose_result = posture_estimator.runEstimator(
        post_processor.run(next_frame.image_results));

    // Put the `frame.id` on the image for debugging purposes
    auto str = std::to_string(next_frame.id);
    cv::putText(next_frame.raw_image, str.c_str(), cv::Point(20, 20),
                cv::FONT_HERSHEY_DUPLEX, 1.0, cv::Scalar(255, 0, 0));

    // printf("post proc. callback %d\n", next_frame.id);
    // std::fflush(stdout);

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
      preprocessed_frames(&this->running, num_inference_core_threads),
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
