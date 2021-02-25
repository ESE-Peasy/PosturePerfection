/**
 * @copyright Copyright (C) 2021  Miklas Riechmann
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

#include "post_processor.h"

namespace IIR {
IIR2ndOrderFilter::IIR2ndOrderFilter(std::vector<float> coefficients) {
  this->nodes = Nodes{
      .b0 = coefficients[0],
      .b1 = coefficients[1],
      .b2 = coefficients[2],
      .a0 = coefficients[3],
      .a1 = coefficients[4],
      .a2 = coefficients[5],
      .tap1 = 0.0,
      .tap2 = 0.0,
  };
}

float IIR2ndOrderFilter::run(float x) {
  float output = this->nodes.b1 * this->nodes.tap1;
  x = x - (this->nodes.a1 * this->nodes.tap1);
  output = output + (this->nodes.b2 * this->nodes.tap2);
  x = x - (this->nodes.a2 * this->nodes.tap2);
  output = output + x * this->nodes.b0;
  this->nodes.tap2 = this->nodes.tap1;
  this->nodes.tap1 = x;
  return output;
}

IIRFilter::IIRFilter(SmoothingSettings smoothing_settings) {
  for (std::vector<float> cs : smoothing_settings.coefficients) {
    this->filters.push_back(IIR2ndOrderFilter(cs));
  }
}

float IIRFilter::run(float x) {
  for (std::vector<IIR2ndOrderFilter>::iterator filter = this->filters.begin();
       filter < this->filters.end(); filter++) {
    x = (*filter).run(x);
  }
  return x;
}

}  // namespace IIR
