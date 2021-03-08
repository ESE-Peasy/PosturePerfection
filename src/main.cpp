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

#include "iir.h"
#include "inference_core.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "post_processor.h"
#include "posture_estimator.h"
#include "pre_processor.h"
#include <QApplication>
#include "mainwindow.h"

#define MODEL_INPUT_X 224
#define MODEL_INPUT_Y 224

void displayImage(cv::Mat originalImage,
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
  cv::imwrite("./testimg.jpg", originalImage);
}

PostProcessing::ProcessedResults pipeline(std::string image) {
  cv::Mat loadedImage = cv::imread(image);

  PreProcessing::PreProcessor preprocessor(MODEL_INPUT_X, MODEL_INPUT_Y);
  Inference::InferenceCore core("models/EfficientPoseRT_LITE.tflite",
                                MODEL_INPUT_X, MODEL_INPUT_Y);

  // Empty settings to disable IIR filtering
  IIR::SmoothingSettings smoothing_settings =
      IIR::SmoothingSettings{std::vector<std::vector<float>>{}};
  PostProcessing::PostProcessor post_processor(0.1, smoothing_settings);

  PreProcessing::PreProcessedImage preprocessed_image =
      preprocessor.run(loadedImage);

  Inference::InferenceResults results = core.run(preprocessed_image);

  PostProcessing::ProcessedResults processed_results =
      post_processor.run(results);

  for (auto body_part : processed_results.body_parts) {
    printf("%f, %f: %s\n", body_part.x, body_part.y,
           (body_part.status == PostProcessing::Trustworthy) ? "Trustworthy"
                                                             : "Untrustworthy");
  }

  // Display image with detected points
  displayImage(loadedImage, processed_results);

  return processed_results;
}

int main(int argc, char *argv[]) {
  printf("Initial Pose Points \n");
  PostProcessing::ProcessedResults ideal_results = pipeline("./person.jpg");
  printf("Current Pose Points \n");
  PostProcessing::ProcessedResults current_results = pipeline("./person2.jpeg");

  PostureEstimating::PostureEstimator e;
  e.update_ideal_pose(ideal_results);
  e.pose_change_threshold = 0;
  bool posture = e.updateCurrentPoseAndCheckPosture(current_results);

  printf("User's posture is %s\n", (posture == true) ? "good" : "bad");

  for (int i = JointMin; i <= JointMax; i++) {
    if (e.pose_changes.joints[i]->upper_angle != 0) {
      printf("Please move your %s -> %s by %f radians or %f degrees\n",
             PostureEstimating::stringJoint(e.pose_changes.joints[i - 1]->joint)
                 .c_str(),
             PostureEstimating::stringJoint(e.pose_changes.joints[i]->joint)
                 .c_str(),
             e.pose_changes.joints[i]->lower_angle,
             e.pose_changes.joints[i]->lower_angle * 360 / (2 * M_PI));
    }
  }
  QApplication a(argc, argv);
  MainWindow w;
  w.showMaximized();
  return a.exec();

}