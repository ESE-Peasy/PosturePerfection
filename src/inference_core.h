/**
 * @file inference_core.h
 * @brief Wrapper API for the TensorFlow Lite model
 *
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

#ifndef SRC_INFERENCE_CORE_H_
#define SRC_INFERENCE_CORE_H_

#include <interpreter_builder.h>
#include <stdint.h>

#include <memory>

#include "pre_processor.h"

namespace Inference {

typedef struct Coordinate {
  float x;
  float y;
  float confidence;
} Coordinate;

typedef struct InferenceResults {
  Coordinate head_top;
  Coordinate upper_neck;
  Coordinate right_shoulder;
  Coordinate right_elbow;
  Coordinate right_wrist;
  Coordinate thorax;
  Coordinate left_shoulder;
  Coordinate left_elbow;
  Coordinate left_wrist;
  Coordinate pelvis;
  Coordinate right_hip;
  Coordinate right_knee;
  Coordinate right_ankle;
  Coordinate left_hip;
  Coordinate left_knee;
  Coordinate left_ankle;
} InferenceResults;

/**
 * @brief A wrapper to make running inference easier
 *
 * This class hides some of the complexity of the underlying TensorFlow Lite C++
 * API. It nonetheless expects preprocessed images. This means images will need
 * to be resized to the model input dimensions and normalised from `uint8_t`
 * values to `float`s in the interval [-1..1] before being run through a
 * `InferenceCore` object.
 *
 */
class InferenceCore {
 private:
  size_t model_input_width;
  size_t model_input_height;
  size_t model_input_size;
  uint8_t model_input_channels = 3;
  std::unique_ptr<tflite::Interpreter> interpreter;
  tflite::StderrReporter error_reporter;
  std::unique_ptr<tflite::FlatBufferModel> model;

  Coordinate pixel_coord_to_Coordinate(uint32_t x, uint32_t y,
                                       float confidence);

 public:
  /**
   * @brief Construct a new Inference Core object
   *
   * @param model_filename Path to the TensorFlow Lite model
   * @param model_input_width Width of the input to the model
   * @param model_input_height Height of the input to the model
   */
  InferenceCore(const char* model_filename, size_t model_input_width,
                size_t model_input_height);

  /**
   * @brief Run a pre-processed image through the loaded model
   *
   * @param preprocessed_image An image that has been resized to the model input
   * dimensions and normalised from `uint8_t` values to `float`s in the interval
   * [-1..1] before being passed to this function
   * @return InferenceResults
   */
  InferenceResults run(PreProcessing::PreProcessedImage preprocessed_image);
};
}  // namespace Inference

#endif  // SRC_INFERENCE_CORE_H_
