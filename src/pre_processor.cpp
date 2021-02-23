/**
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
#include "pre_processor.h"

#include <memory>

namespace PreProcessing {

PreProcessor::PreProcessor(size_t model_width, size_t model_height)
    : model_width(model_width), model_height(model_height) {}

uint8_t* PreProcessor::resize(cv::Mat cv_image) {
  cv::resize(cv_image, cv_image, cv::Size(model_width, model_height));
  return cv_image.data;
}

float* PreProcessor::normalise(uint8_t* resized_image) {
  int size = model_width * model_height;
  int step = 3;

  for (int i = 0; i < size * step; i += step) {
    // - Scale to [-1..1]
    // - OpenCV uses BGR but the model uses RGB, therefore
    //   the channels must also be switched.
    uint8_t* img = &(resized_image[i]);
    preprocessed_image[i + 0] = img[2] / 127.5 - 1;  // Switch R and B channels
    preprocessed_image[i + 1] = img[1] / 127.5 - 1;
    preprocessed_image[i + 2] = img[0] / 127.5 - 1;  // Switch B and R channels
  }

  return preprocessed_image;
}

PreProcessedImage PreProcessor::run(cv::Mat cv_image) {
  uint8_t* resized_image = resize(cv_image);

  float* image = normalise(resized_image);

  return PreProcessedImage{image};
}
}  // namespace PreProcessing