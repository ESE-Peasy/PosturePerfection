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
#include <mutex>
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

// ============================================================================

class PreprocessedDeque {
 private:
  std::mutex lock;
  std::deque<std::tuple<uint8_t, cv::Mat, PreProcessing::PreProcessedImage>>
      queue;

 public:
  void push_back(
      std::tuple<uint8_t, cv::Mat, PreProcessing::PreProcessedImage> frame) {
    this->queue.push_back(frame);
  }

  std::tuple<uint8_t, cv::Mat, PreProcessing::PreProcessedImage> pop_front(
      void) {
    std::tuple<uint8_t, cv::Mat, PreProcessing::PreProcessedImage> front;
    while (run_flag) {
      lock.lock();
      if (!this->queue.empty()) {
        front = this->queue.front();
        this->queue.pop_front();
        lock.unlock();
        break;
      }
      lock.unlock();
    }
    return front;
  }
};

void input_fn2(std::string image, PreprocessedDeque* preprocessed_images,
               PreProcessing::PreProcessor* preprocessor) {
  cv::VideoCapture cap(0);
  if (!cap.isOpened()) {
    printf("Can't access camera\n");
    return;
  }

  uint8_t id = 0;

  while (run_flag) {
    cv::Mat frame;
    cap.read(frame);
    if (frame.empty()) {
      printf("Empty frame\n");
      return;
    }

    preprocessed_images->push_back(
        std::tuple<uint8_t, cv::Mat, PreProcessing::PreProcessedImage>{
            id++, frame, preprocessor->run(frame)});

    std::this_thread::sleep_for(std::chrono::milliseconds(105));
  }
}

class CoreResultsDeque {
 private:
  std::mutex lock;
  std::deque<std::tuple<uint8_t, cv::Mat, Inference::InferenceResults>> queue;
  uint8_t id = -1;

 public:
  void push_back(
      std::tuple<uint8_t, cv::Mat, Inference::InferenceResults> next_frame) {
    while (run_flag) {
      this->lock.lock();
      if ((uint8_t)(this->id + 1) == std::get<0>(next_frame)) {
        this->queue.push_back(next_frame);
        this->id++;
        this->lock.unlock();
        break;
      }
      this->lock.unlock();
    }
  }

  void pop_front(void) { this->queue.pop_front(); }

  bool empty(void) { return this->queue.empty(); }

  std::tuple<uint8_t, cv::Mat, Inference::InferenceResults> front(void) {
    return this->queue.front();
  }
};

void process_frames_fn2(int name, PreprocessedDeque* preprocessed_images,
                        CoreResultsDeque* core_results,
                        Inference::InferenceCore core) {
  while (run_flag) {
    // printf("%d: about to start\n", name);
    auto next_frame = preprocessed_images->pop_front();
    // printf("%d: got next frame\n", name);

    // printf("%d: running core\n", name);
    auto core_result = core.run(std::get<2>(next_frame));
    // printf("%d: core returned\n", name);

    core_results->push_back(
        std::tuple<uint8_t, cv::Mat, Inference::InferenceResults>{
            std::get<0>(next_frame), std::get<1>(next_frame), core_result});
    // printf("%d: done\n", name);
  }
}

void post_processing_fn2(
    CoreResultsDeque* core_results,
    std::deque<std::tuple<uint8_t, cv::Mat, PostProcessing::ProcessedResults>>*
        processed_results,
    PostProcessing::PostProcessor* post_processor) {
  while (run_flag) {
    if (core_results->empty()) {
      continue;
    }
    // printf("post processing\n");
    auto next_frame = core_results->front();

    processed_results->push_back(
        std::tuple<uint8_t, cv::Mat, PostProcessing::ProcessedResults>{
            std::get<0>(next_frame), std::get<1>(next_frame),
            post_processor->run(std::get<2>(next_frame))});
    core_results->pop_front();
  }
}

void pipeline_threaded2(std::string image) {
  run_flag = true;
  // Set up pipeline
  std::vector<std::thread> threads;
  PreProcessing::PreProcessor preprocessor(MODEL_INPUT_X, MODEL_INPUT_Y);
  IIR::SmoothingSettings smoothing_settings =
      IIR::SmoothingSettings{std::vector<std::vector<float>>{}};
  PostProcessing::PostProcessor post_processor(0.1, smoothing_settings);

  // Queues
  PreprocessedDeque preprocessed_images;
  CoreResultsDeque core_results;
  std::deque<std::tuple<uint8_t, cv::Mat, PostProcessing::ProcessedResults>>
      processed_results;

  std::thread input(&input_fn2, image, &preprocessed_images, &preprocessor);
  threads.push_back(std::move(input));

  for (int i = 0; i < 4; i++) {
    Inference::InferenceCore core("models/EfficientPoseRT_LITE.tflite",
                                  MODEL_INPUT_X, MODEL_INPUT_Y);

    std::thread process_frames(&process_frames_fn2, i, &preprocessed_images,
                               &core_results, std::move(core));
    threads.push_back(std::move(process_frames));
  }

  std::thread post_process(&post_processing_fn2, &core_results,
                           &processed_results, &post_processor);
  threads.push_back(std::move(post_process));

  int i = 0;
  bool flag = true;
  while (flag) {
    if (processed_results.empty()) {
      continue;
    }

    auto next_frame = processed_results.front();

    flag = displayImage(std::get<1>(next_frame), std::get<2>(next_frame));

    processed_results.pop_front();
    i++;
    if (i >= NUM_LOOPS) break;
  }

  printf("DONE\n");

  run_flag = false;
  for (auto& t : threads) {
    t.join();
  }
}

// ============================================================================

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

void pipeline_threaded(std::string image) {
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
  // std::deque<cv::Mat> input_frames;
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

    // flag = displayImage(next_frame.first, next_frame.second);

    processed_results.pop_front();
    i++;
    if (i >= NUM_LOOPS) break;
  }

  run_flag = false;
  input.join();
  process_frames.join();
  post_process.join();
}

// ============================================================================

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
  // pipeline_threaded("./person.jpg");
  pipeline_threaded2("./person.jpg");
}
