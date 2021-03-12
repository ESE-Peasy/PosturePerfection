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
    cv::Mat frame;
    cap.read(frame);
    if (frame.empty()) {
      fprintf(stderr, "Empty frame\n");
      return;
    }

    preprocessed_frames.push(
        PreprocessedFrame{id++, frame, preprocessor.run(frame)});

    // Set the time to something appropriate for the system. This dictates the
    // frame-rate at which the system operates.
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

void Pipeline::core_thread_body(Inference::InferenceCore core) {
  while (running) {
    auto next_frame = preprocessed_frames.pop();

    auto core_result = core.run(next_frame.preprocessed_image);

    core_results.push(
        CoreResults{next_frame.id, next_frame.raw_image, core_result});
  }
}

void Pipeline::post_processing_thread_body() {
  while (running) {
    auto next_frame = core_results.pop();

    processed_results.push(
        ProcessedResults{next_frame.id, next_frame.raw_image,
                         post_processor.run(next_frame.image_results)});
  }
}

// Pipeline::Pipeline(uint8_t num_inference_core_threads, void(*callback)())
Pipeline::Pipeline(uint8_t num_inference_core_threads)
    : preprocessor(MODEL_INPUT_X, MODEL_INPUT_Y),
      // Disable smoothing with empty settings
      post_processor(0.1,
                     IIR::SmoothingSettings{std::vector<std::vector<float>>{}}),
      preprocessed_frames(&this->running),
      core_results(&this->running),
      processed_results(&this->running) {
  this->running = true;

  std::thread input_thread(&Pipeline::Pipeline::input_thread_body, this);
  threads.push_back(std::move(input_thread));

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
}  // namespace Pipeline
