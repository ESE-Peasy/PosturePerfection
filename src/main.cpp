/**
 * @copyright Copyright (C) 2021  Miklas Riechmann, Ashwin Maliampurakal
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

#include <stdio.h>
#include <time.h>

#include <chrono>  // NOLINT [build/c++11]
#include <deque>
#include <iostream>
#include <thread>  //NOLINT [build/c++11]
#include <utility>

#include "iir.h"
#include "inference_core.h"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "post_processor.h"
#include "posture_estimator.h"
#include "pre_processor.h"

#define MODEL_INPUT_X 224
#define MODEL_INPUT_Y 224

#define NUM_LOOPS 500

bool run_flag = true;

bool displayImage(cv::Mat originalImage,
                  PostProcessing::ProcessedResults processed_results) {
  cv::Scalar blue(255, 0, 0);
  cv::Scalar red(0, 0, 255);
  int imageWidth = originalImage.cols;
  int imageHeight = originalImage.rows;
  int circleRadius = 5;

  for (auto body_part : processed_results.body_parts) {
    if (body_part.status == PostProcessing::Trustworthy) {
      cv::circle(originalImage,
                 cv::Point(static_cast<int>(body_part.x * imageWidth),
                           static_cast<int>(body_part.y * imageHeight)),
                 circleRadius, blue, -1);
    } else {
      cv::circle(originalImage,
                 cv::Point(static_cast<int>(body_part.x * imageWidth),
                           static_cast<int>(body_part.y * imageHeight)),
                 circleRadius, red, -1);
    }
  }

  // Save the image with detected points
  cv::imshow("Live", originalImage);
  if (cv::waitKey(5) >= 0) {
    return false;
  } else {
    return true;
  }
}

void input_fn(std::string image,
              std::deque<std::pair<cv::Mat, PreProcessing::PreProcessedImage>>*
                  preprocessed_images,
              PreProcessing::PreProcessor* preprocessor) {
  cv::VideoCapture cap(0);
  if (!cap.isOpened()) {
    printf("Can't access camera\n");
    return;
  }

  while (run_flag) {
    cv::Mat frame;
    cap.read(frame);
    if (frame.empty()) {
      printf("Empty frame\n");
      return;
    }

    preprocessed_images->push_back(
        std::pair<cv::Mat, PreProcessing::PreProcessedImage>(
            frame, preprocessor->run(frame)));

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }
}

void process_frames_fn(
    std::deque<std::pair<cv::Mat, PreProcessing::PreProcessedImage>>*
        preprocessed_images,
    std::deque<std::pair<cv::Mat, Inference::InferenceResults>>* core_results,
    Inference::InferenceCore* core) {
  while (run_flag) {
    if (preprocessed_images->empty()) {
      continue;
    }
    auto next_frame = preprocessed_images->front();

    core_results->push_back(std::pair<cv::Mat, Inference::InferenceResults>(
        next_frame.first, core->run(next_frame.second)));

    preprocessed_images->pop_front();
  }
}

void post_processing_fn(
    std::deque<std::pair<cv::Mat, Inference::InferenceResults>>* core_results,
    std::deque<std::pair<cv::Mat, PostProcessing::ProcessedResults>>*
        processed_results,
    PostProcessing::PostProcessor* post_processor) {
  while (run_flag) {
    if (core_results->empty()) {
      continue;
    }
    auto next_frame = core_results->front();

    processed_results->push_back(
        std::pair<cv::Mat, PostProcessing::ProcessedResults>(
            next_frame.first, post_processor->run(next_frame.second)));
    core_results->pop_front();
  }
}

void pipeline_threaded2(std::string image) {
  run_flag = true;
  // Set up pipeline
  PreProcessing::PreProcessor preprocessor(MODEL_INPUT_X, MODEL_INPUT_Y);
  Inference::InferenceCore core("models/EfficientPoseRT_LITE.tflite",
                                MODEL_INPUT_X, MODEL_INPUT_Y);
  // Empty settings to disable IIR filtering
  IIR::SmoothingSettings smoothing_settings =
      IIR::SmoothingSettings{std::vector<std::vector<float>>{}};
  PostProcessing::PostProcessor post_processor(0.1, smoothing_settings);

  // Queues
  std::deque<cv::Mat> input_frames;
  std::deque<std::pair<cv::Mat, PreProcessing::PreProcessedImage>>
      preprocessed_images;
  std::deque<std::pair<cv::Mat, Inference::InferenceResults>> core_results;
  std::deque<std::pair<cv::Mat, PostProcessing::ProcessedResults>>
      processed_results;

  std::thread input(&input_fn, image, &preprocessed_images, &preprocessor);

  std::thread process_frames(&process_frames_fn, &preprocessed_images,
                             &core_results, &core);

  std::thread post_process(&post_processing_fn, &core_results,
                           &processed_results, &post_processor);

  int i = 0;
  bool flag = true;
  while (flag) {
    if (processed_results.empty()) {
      continue;
    }

    auto next_frame = processed_results.front();

    flag = displayImage(next_frame.first, next_frame.second);

    processed_results.pop_front();
    // i++;
    // if (i >= NUM_LOOPS) break;
  }

  run_flag = false;
  input.join();
  process_frames.join();
  post_process.join();
}

void pipeline(std::string image) {
  // Set up pipeline
  PreProcessing::PreProcessor preprocessor(MODEL_INPUT_X, MODEL_INPUT_Y);
  Inference::InferenceCore core("models/EfficientPoseRT_LITE.tflite",
                                MODEL_INPUT_X, MODEL_INPUT_Y);
  // Empty settings to disable IIR filtering
  IIR::SmoothingSettings smoothing_settings =
      IIR::SmoothingSettings{std::vector<std::vector<float>>{}};
  PostProcessing::PostProcessor post_processor(0.1, smoothing_settings);

  cv::VideoCapture cap(0);
  if (!cap.isOpened()) {
    printf("Can't access camera\n");
    return;
  }

  // Read input in loop
  for (int i = 0; i < NUM_LOOPS; i++) {
    cv::Mat frame;
    cap.read(frame);
    if (frame.empty()) {
      printf("Empty frame\n");
      return;
    }

    auto now = std::chrono::system_clock::now();

    PreProcessing::PreProcessedImage preprocessed_image =
        preprocessor.run(frame);

    Inference::InferenceResults results = core.run(preprocessed_image);

    PostProcessing::ProcessedResults processed_results =
        post_processor.run(results);

    std::this_thread::sleep_until(now + std::chrono::milliseconds(200));
  }
}

int main(int argc, char const* argv[]) {
  printf("start\n");
  // pipeline("./person.jpg");
  pipeline_threaded2("./person.jpg");
}
