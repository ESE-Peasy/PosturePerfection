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

#include "../post_processor.h"

namespace IIR {
IIR2ndOrderFilter::IIR2ndOrderFilter(float* coefficients) {
  this->nodes.b0 = coefficients[0];
  this->nodes.b1 = coefficients[1];
  this->nodes.b2 = coefficients[2];
  this->nodes.a0 = coefficients[3];
  this->nodes.a1 = coefficients[4];
  this->nodes.a2 = coefficients[5];
  this->nodes.tap1 = 0.0;
  this->nodes.tap2 = 0.0;
}

IIR2ndOrderFilter::run(float x) {
  float output = this->nodes.b1 * this->nodes.tap1;
  x = x - (this->nodes.a1 * this->nodes.tap1);
  output = output + (this->nodes.b2 * this->nodes.tap2);
  x = x - (this->nodes.a2 * this->nodes.tap2);
  output = output + x * this->nodes.b0;
  this->nodes.tap2 = this->nodes.tap1;
  this->nodes.tap1 = x;
  return output;
}

IIRFilter::IIRFilter(PostProcessing::ProcessedResults smoothing_settings) {
  this->filters =
      (float*)malloc(sizeof(IIR2ndOrderFilter) * smoothing_settings.num_stages);
  for (int i = 0; i < smoothing_settings.num_stages; i++) {
    IIR2ndOrderFilter(smoothing_settings.sos_coefficients[i]);
  }
  this->num_stages = smoothing_settings.num_stages;
}

IIRFilter::~IIRFilter() { free(this->filters); }

IIRFilter::run(float x) {
  for (int i = 0; i < this->num_stages; i++) {
    x = this->filters[i].run(x);
  }
  return x;
}

}  // namespace IIR
