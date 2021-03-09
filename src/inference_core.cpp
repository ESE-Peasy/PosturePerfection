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

#include "inference_core.h"

#include <create_op_resolver.h>
#include <model_builder.h>
#include <op_resolver.h>
#include <stderr_reporter.h>
#include <stdio.h>

#include <future>
#include <memory>

#include "intermediate_structures.h"

typedef struct Result {
  float confidence;
  size_t x;
  size_t y;
} Result;

namespace Inference {

InferenceCore::InferenceCore(const char* model_filename,
                             size_t model_input_width,
                             size_t model_input_height) {
  this->model_input_width = model_input_width;
  this->model_input_height = model_input_height;
  this->model_input_size = model_input_width * model_input_height;

  // Load the model
  auto model =
      tflite::FlatBufferModel::BuildFromFile(model_filename, &error_reporter);
  this->model.swap(model);

  auto resolver = tflite::CreateOpResolver();

  // Start the interpreter
  if (tflite::InterpreterBuilder(
          *(this->model), *resolver)(&(this->interpreter)) != kTfLiteOk) {
    // Return failure.
    fprintf(stderr, "Could not start interpreter\n");
    exit(EXIT_FAILURE);
  }

  this->interpreter->AllocateTensors();
}

Coordinate InferenceCore::pixel_coord_to_Coordinate(uint32_t x, uint32_t y,
                                                    float confidence) {
  return Coordinate{
      static_cast<float>(x) / static_cast<float>(this->model_input_width),
      static_cast<float>(y) / static_cast<float>(this->model_input_height),
      confidence};
}

InferenceResults InferenceCore::run(
    PreProcessing::PreProcessedImage preprocessed_image) {
  // Initialise and get pointer to input
  auto input = this->interpreter->typed_input_tensor<float>(0);

  size_t size = this->model_input_size;

  // Copy the image to the input
  memcpy(
      input, preprocessed_image.image,
      size * this->model_input_channels * sizeof(preprocessed_image.image[0]));

  // Free the memory allocated for the image as it is no longer needed
  free(preprocessed_image.image);

  // Run the model
  this->interpreter->Invoke();

  // Get pointer to output
  auto output = this->interpreter->typed_output_tensor<float>(0);

  Result results[BodyPartMax + 1];
  memset(results, 0, sizeof(results));

  size_t step = BodyPartMax + 1;
  size_t width = this->model_input_width;
  size_t height = this->model_input_height;

  // Go through output to find where largest confidence for each body part is
  // Step through each pixel
  for (uint32_t i = 0; i < size * step; i += step) {
    // Look at predictions for each body part in current pixel
    for (int body_part = BodyPartMin; body_part <= BodyPartMax; body_part++) {
      float out = output[i + body_part];
      if (out > results[body_part].confidence) {
        results[body_part] = {out, (i / step) % width, (i / step) / height};
      }
    }
  }

  InferenceResults results_out;

  int body_part_index = BodyPartMin;
  for (auto& body_part : results_out.body_parts) {
    body_part = pixel_coord_to_Coordinate(results[body_part_index].x,
                                          results[body_part_index].y,
                                          results[body_part_index].confidence);
    body_part_index++;
  }

  return results_out;
}

InferenceResults InferenceCore::run_async(
    std::future<PreProcessing::PreProcessedImage> preprocessed_image_future) {
  return this->run(preprocessed_image_future.get());
}
}  // namespace Inference
