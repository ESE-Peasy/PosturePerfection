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
#include "pre_processor.h"

#define MODEL_INPUT_X 224
#define MODEL_INPUT_Y 224

int main(int argc, char const *argv[]) {
  PreProcessing::PreProcessor preprocessor(500, 500, 224, 224);

  PreProcessing::PreProcessedImage preprocessed_image =
      preprocessor.run(image.pixel_data);

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
}
