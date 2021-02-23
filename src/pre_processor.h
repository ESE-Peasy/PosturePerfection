/**
 * @file pre_processor.h
 * @brief Interface for pre processing the input image before passing to
 * `Inference::InferenceCore`
 *
 * @copyright Copyright (C) 2021  Ashwin Maliampurakal
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

#ifndef SRC_PRE_PROCESSOR_H_
#define SRC_PRE_PROCESSOR_H_

#include <stdint.h>

#include <memory>

#include "opencv2/opencv.hpp"

namespace PreProcessing {

/**
 * @brief A structure of the pre processed image
 *
 * After pre processing, the image will be resized and normalised before
 * being passed to the `Inference::InferenceCore` as a `PreProcessedImage`
 *
 */
struct PreProcessedImage {
  float* image;
};

/**
 * @brief PreProcess the input image before passing to
 * `Inference::InferenceCore`
 *
 * This class contains some pre processing steps to allow an input image to be
 * passed to the `Inference::InferenceCore`. The following operations are
 * performed to process the raw data:
 * - Resize the image to fit the dimensions of the model being used
 * - Normalise the image pixels to the range [-1..1]
 *
 */
class PreProcessor {
 private:
  size_t model_width;
  size_t model_height;
  float preprocessed_image[224 * 224 * 3 + 1];

  /**
   * @brief Resize the input image to the desired dimensions
   *
   * @param cv_image The OpenCV image to be preprocessed
   * @return The resized array of pixels of the input image
   */
  uint8_t* resize(cv::Mat cv_image);

  /**
   * @brief Normalise the input image to the required interval [-1..1]
   *
   * @param resized_image Array of pixels of the resized input image
   * @return The normalised array of pixels of the input image
   */
  float* normalise(uint8_t* resized_image);

 public:
  /**
   * @brief Construct a new PreProcessor object
   *
   * @param model_width Desired width of the resized image (should match
   * `Inference::InferenceCore::model_input_width`)
   * @param model_height Desired height of the resized image (should match
   * `Inference::InferenceCore::model_input_height`)
   * @return The resized array of pixels of the input image
   */
  PreProcessor(size_t model_width, size_t model_height);

  /**
   * @brief Apply pre processing to the given input image
   *
   * @param cv_image The OpenCV image to be preprocessed
   * @return PreprocessedImage after resizing and normalising
   */
  PreProcessedImage run(cv::Mat cv_image);
};

}  // namespace PreProcessing
#endif  // SRC_PRE_PROCESSOR_H_
