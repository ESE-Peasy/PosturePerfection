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
#include "post_processor.h"
#include "posture_in.h"  // Hardcoded image for testing

#define MODEL_INPUT_X 224
#define MODEL_INPUT_Y 224

int main(int argc, char const *argv[]) {
  float preprocessed_image[224 * 224 * 3 + 1];

  int size = MODEL_INPUT_X * MODEL_INPUT_Y;
  int step = 3;

  for (int i = 0; i < size * step; i += step) {
    const uint8_t *img = &(image.pixel_data[i]);
    // Scale to [-1..1]
    preprocessed_image[i + 0] = img[0] / 127.5 - 1;
    preprocessed_image[i + 1] = img[1] / 127.5 - 1;
    preprocessed_image[i + 2] = img[2] / 127.5 - 1;
  }

  Inference::InferenceCore core("models/EfficientPoseRT_LITE.tflite",
                                MODEL_INPUT_X, MODEL_INPUT_Y);

  // Empty settings to disable IIR filtering
  IIR::SmoothingSettings smoothing_settings =
      IIR::SmoothingSettings{std::vector<std::vector<float>>{}};
  PostProcessing::PostProcessor post_processor(0.1, smoothing_settings);

  Inference::InferenceResults results =
      core.run(PreProcessing::PreProcessedImage{preprocessed_image});

  PostProcessing::ProcessedResults processed_results =
      post_processor.run(results);

  for (auto body_part : processed_results.body_parts) {
    printf("%f, %f: %s\n", body_part.x, body_part.y,
           (body_part.status == PostProcessing::Trustworthy) ? "Trustworthy"
                                                             : "Untrustworthy");
  }
}
