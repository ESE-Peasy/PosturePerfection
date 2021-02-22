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

PreProcessor::PreProcessor(size_t input_image_width, size_t input_image_height,
                           size_t model_width, size_t model_height)
    : input_image_width(input_image_width),
      input_image_height(input_image_height),
      model_width(model_width),
      model_height(model_height) {}

uint8_t* PreProcessor::resize(uint8_t* input_image) {}

float* PreProcessor::normalise(uint8_t* resized_image) {
  float normalised_image[model_width * model_height * 3 + 1];

  int size = model_width * model_height;
  int step = 3;

  for (int i = 0; i < size * step; i += step) {
    // Scale to [-1..1]
    uint8_t* img = &(resized_image[i]);
    printf("%u ", img);
    normalised_image[i + 0] = img[0] / 127.5 - 1;
    normalised_image[i + 1] = img[1] / 127.5 - 1;
    normalised_image[i + 2] = img[2] / 127.5 - 1;
  }

  return normalised_image;
}

PreProcessedImage PreProcessor::run(uint8_t* input_image) {
  float* image = normalise(input_image);

  return PreProcessedImage{image};
}
}  // namespace PreProcessing