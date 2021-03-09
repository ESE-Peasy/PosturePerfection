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

#include <time.h>

#include <deque>
#include <future>
#include <utility>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
#include <stdio.h>

#include "iir.h"
#include "inference_core.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "post_processor.h"
#include "posture_estimator.h"
#include "pre_processor.h"

#define MODEL_INPUT_X 224
#define MODEL_INPUT_Y 224

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

clock_t pipeline_threaded(std::string image) {
  // Set up pipeline
  PreProcessing::PreProcessor preprocessor(MODEL_INPUT_X, MODEL_INPUT_Y);
  Inference::InferenceCore core("models/EfficientPoseRT_LITE.tflite",
                                MODEL_INPUT_X, MODEL_INPUT_Y);
  // Empty settings to disable IIR filtering
  IIR::SmoothingSettings smoothing_settings =
      IIR::SmoothingSettings{std::vector<std::vector<float>>{}};
  PostProcessing::PostProcessor post_processor(0.1, smoothing_settings);

  std::deque<std::future<PostProcessing::ProcessedResults>> output_futures;

  clock_t t_start = clock();

  // Read input in loop
  for (int i = 0; i < 50; i++) {
    // Get input image
    cv::Mat loadedImage = cv::imread(image);
    std::promise<cv::Mat> loadedImage_promise;
    auto loadedImage_future = loadedImage_promise.get_future();
    loadedImage_promise.set_value(loadedImage);

    std::future<PreProcessing::PreProcessedImage> preprocessed_image_future =
        std::async(std::launch::async, &PreProcessing::PreProcessor::run_async,
                   &preprocessor, std::move(loadedImage_future));

    std::future<Inference::InferenceResults> results_future =
        std::async(std::launch::async, &Inference::InferenceCore::run_async,
                   &core, std::move(preprocessed_image_future));

    std::future<PostProcessing::ProcessedResults> processed_results_future =
        std::async(std::launch::async,
                   &PostProcessing::PostProcessor::run_async, &post_processor,
                   std::move(results_future));

    // auto processed_results = processed_results_future.get();
    // output_futures.push_back(std::move(processed_results_future));
    // processed_results_future.get();

    // if (output_futures.size() > 10) {
    //   output_futures.pop_front();
    // }
  }
  // for (auto& future : output_futures) {
  //   future.get();
  // }
  auto time = clock() - t_start;
  return time;
}

void generate_input_frames_fn(std::string image,
                              std::deque<cv::Mat>* input_frames) {
  // for (int i = 0; i < 500; i++) {
  //   input_frames->push_back(cv::imread(image));
  // }
  cv::VideoCapture cap(0);
  if (!cap.isOpened()) {
    printf("Can't access camera\n");
    return;
  }
  cv::Mat frame;
  while (run_flag) {
    cap.read(frame);
    if (frame.empty()) {
      printf("Empty frame\n");
      return;
    }
    input_frames->push_back(frame);
  }
}

void preprocessed_images_fn(
    std::deque<cv::Mat>* input_frames,
    std::deque<std::pair<cv::Mat, PreProcessing::PreProcessedImage>>*
        preprocessed_images,
    PreProcessing::PreProcessor* preprocessor) {
  while (run_flag) {
    if (input_frames->empty()) {
      continue;
    }
    auto input_frame = input_frames->front();

    preprocessed_images->push_back(
        std::pair<cv::Mat, PreProcessing::PreProcessedImage>(
            input_frame, preprocessor->run(input_frame)));

    input_frames->pop_front();
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

  // Read input in loop
  std::thread generate_input_frames(&generate_input_frames_fn, image,
                                    &input_frames);

  std::thread generate_preprocessed_images(&preprocessed_images_fn,
                                           &input_frames, &preprocessed_images,
                                           &preprocessor);

  std::thread process_frames(&process_frames_fn, &preprocessed_images,
                             &core_results, &core);

  std::thread post_process(&post_processing_fn, &core_results,
                           &processed_results, &post_processor);

  bool flag = true;
  while (flag) {
    if (processed_results.empty()) {
      continue;
    }

    auto next_frame = processed_results.front();

    flag = displayImage(next_frame.first, next_frame.second);

    processed_results.pop_front();
  }

  // while (processed_results.size() < 500) {
  //   if (processed_results.empty()) {
  //     continue;
  //   }

  //   for (auto body_part : processed_results.back().body_parts) {
  //     printf("%f, %f: %s\n", body_part.x, body_part.y,
  //            (body_part.status == PostProcessing::Trustworthy)
  //                ? "Trustworthy"
  //                : "Untrustworthy");
  //   }
  // };

  run_flag = false;
  generate_input_frames.join();
  generate_preprocessed_images.join();
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

  // Read input in loop
  for (int i = 0; i < 500; i++) {
    // Get input image
    cv::Mat loadedImage = cv::imread(image);

    PreProcessing::PreProcessedImage preprocessed_image =
        preprocessor.run(loadedImage);

    Inference::InferenceResults results = core.run(preprocessed_image);

    PostProcessing::ProcessedResults processed_results =
        post_processor.run(results);
  }
}

int main(int argc, char const* argv[]) {
  // printf("Initial Pose Points \n");

  // Run each twice in case timing differs due to caching
  // auto tx = clock();
  // pipeline("./person.jpg");
  // auto t1 = clock() - tx;

  // tx = clock();
  // pipeline("./person.jpg");
  // pipeline("./person.jpg");
  // pipeline("./person.jpg");
  // auto t2 = clock() - tx;

  // tx = clock();
  // auto t3 = clock() - tx;

  // tx = clock();
  printf("start\n");
  pipeline_threaded2("./person.jpg");
  // printf("done 1\n");
  // pipeline_threaded2("./person.jpg");
  // printf("done 2\n");
  // pipeline_threaded2("./person.jpg");
  // printf("done 3\n");
  // auto t4 = clock() - tx;

  // printf("Single-threaded approach: t1=%.2fs; t2=%.2fs\n",
  //        t1 / (float)CLOCKS_PER_SEC, t2 / (float)CLOCKS_PER_SEC);
  // printf("Multi-threaded approach: t3=%.2fs; t4=%.2fs\n",
  //        t3 / (float)CLOCKS_PER_SEC, t4 / (float)CLOCKS_PER_SEC);

  // printf("Current Pose Points \n");
  // pipeline("./person2.jpeg");

  // PostureEstimating::PostureEstimator e;
  // e.update_ideal_pose(ideal_results);
  // e.pose_change_threshold = 0;
  // bool posture = e.updateCurrentPoseAndCheckPosture(current_results);

  // // printf("User's posture is %s\n", (posture == true) ? "good" : "bad");

  // for (int i = JointMin; i <= JointMax; i++) {
  //   if (e.pose_changes.joints[i]->upper_angle != 0) {
  //     printf("Please move your %s -> %s by %f radians or %f degrees\n",
  //            PostureEstimating::stringJoint(e.pose_changes.joints[i -
  //            1]->joint)
  //                .c_str(),
  //            PostureEstimating::stringJoint(e.pose_changes.joints[i]->joint)
  //                .c_str(),
  //            e.pose_changes.joints[i]->lower_angle,
  //            e.pose_changes.joints[i]->lower_angle * 360 / (2 * M_PI));
  //   }
  // }
}
