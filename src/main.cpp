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
#include "pre_processor.h"

#define MODEL_INPUT_X 224
#define MODEL_INPUT_Y 224

void displayImage(cv::Mat originalImage, Inference::InferenceResults results) {
  cv::Scalar blue(255, 0, 0);

  cv::circle(originalImage,
             cv::Point(static_cast<int>(results.head_top.x * imageWidth),
                       static_cast<int>(results.head_top.y * imageHeight)),
             circleRadius, blue, -1);
  cv::circle(originalImage,
             cv::Point(static_cast<int>(results.upper_neck.x * imageWidth),
                       static_cast<int>(results.upper_neck.y * imageHeight)),
             circleRadius, blue, -1);
  cv::circle(
      originalImage,
      cv::Point(static_cast<int>(results.right_shoulder.x * imageWidth),
                static_cast<int>(results.right_shoulder.y * imageHeight)),
      circleRadius, blue, -1);
  cv::circle(originalImage,
             cv::Point(static_cast<int>(results.pelvis.x * imageWidth),
                       static_cast<int>(results.pelvis.y * imageHeight)),
             circleRadius, blue, -1);
  cv::circle(originalImage,
             cv::Point(static_cast<int>(results.right_knee.x * imageWidth),
                       static_cast<int>(results.right_knee.y * imageHeight)),
             circleRadius, blue, -1);

  // Save the image with detected points
  cv::imwrite("./testimg.jpg", originalImage);
}

int main(int argc, char const *argv[]) {
  cv::Mat loadedImage = cv::imread("./person.jpg");

  PreProcessing::PreProcessor preprocessor(MODEL_INPUT_X, MODEL_INPUT_Y);
  Inference::InferenceCore core("models/EfficientPoseRT_LITE.tflite",
                                MODEL_INPUT_X, MODEL_INPUT_Y);
  PostProcessing::PostProcessor post_processor(0.1, smoothing_settings);

  PreProcessing::PreProcessedImage preprocessed_image = preprocessor.run(frame);

  Inference::InferenceResults results = core.run(preprocessed_image);

  PostProcessing::ProcessedResults processed_results =
      post_processor.run(results);

  for (auto body_part : processed_results.body_parts) {
    printf("%f, %f: %s\n", body_part.x, body_part.y,
           (body_part.status == PostProcessing::Trustworthy) ? "Trustworthy"
                                                             : "Untrustworthy");
  }
  // Display image with detected points
  // displayImage(loadedImage, results);
}
