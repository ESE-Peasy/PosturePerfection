/**
 * @copyright Copyright (C) 2021  Miklas Riechmann, Ashwin Maliampurakal
 *
 *  This program is free software: you can blueistribute it and/or modify
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
  std::string windowName = "Inference Results";

  cv::namedWindow(windowName);
  cv::Scalar blue(255, 0, 0);

  cv::circle(originalImage,
             cv::Point((int)(results.head_top.x * originalImage.cols),
                       (int)(results.head_top.y * originalImage.rows)),
             5, blue, -1);
  cv::circle(originalImage,
             cv::Point((int)(results.upper_neck.x * originalImage.cols),
                       (int)(results.upper_neck.y * originalImage.rows)),
             5, blue, -1);
  cv::circle(originalImage,
             cv::Point((int)(results.right_shoulder.x * originalImage.cols),
                       (int)(results.right_shoulder.y * originalImage.rows)),
             5, blue, -1);
  cv::circle(originalImage,
             cv::Point((int)(results.pelvis.x * originalImage.cols),
                       (int)(results.pelvis.y * originalImage.rows)),
             5, blue, -1);
  cv::circle(originalImage,
             cv::Point((int)(results.right_knee.x * originalImage.cols),
                       (int)(results.right_knee.y * originalImage.rows)),
             5, blue, -1);

  cv::imshow(windowName, originalImage);

  cv::waitKey(5);
}

int main(int argc, char const *argv[]) {
  // Setup the camera input
  cv::VideoCapture cap(0);

  if (!cap.isOpened()) {
    printf("Error opening camera");
    return 0;
  }

  // Initialise the PreProcessor and InferenceCore
  PreProcessing::PreProcessor preprocessor(MODEL_INPUT_X, MODEL_INPUT_Y);
  Inference::InferenceCore core("models/EfficientPoseRT_LITE.tflite",
                                MODEL_INPUT_X, MODEL_INPUT_Y);

  // Empty settings to disable IIR filtering
  IIR::SmoothingSettings smoothing_settings =
      IIR::SmoothingSettings{std::vector<std::vector<float>>{}};
  PostProcessing::PostProcessor post_processor(0.1, smoothing_settings);

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
