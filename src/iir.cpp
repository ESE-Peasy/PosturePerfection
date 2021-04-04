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

#define MAX_SETUP_ITERS 1000
#define NUM_CONSECUTIVE_SETUP_SAMPLES 3
#define SETUP_ERROR 0.01

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
  float output = nodes.b1 * nodes.tap1;
  x = x - (nodes.a1 * nodes.tap1);
  output = output + (nodes.b2 * nodes.tap2);
  x = x - (nodes.a2 * nodes.tap2);
  output = output + x * nodes.b0;
  nodes.tap2 = nodes.tap1;
  nodes.tap1 = x;
  return output;
}

IIRFilter::IIRFilter(SmoothingSettings smoothing_settings) {
  for (std::vector<float> cs : smoothing_settings.coefficients) {
    this->filters.push_back(IIR2ndOrderFilter(cs));
  }
}

void IIRFilter::set(float x) {
  // It was difficult to find an analytical, general solution for setting the
  // initial state, so we have opted for running the filter until it has reached
  // the desired initial state
  float result;

  const float lower_bound = x * (1 - SETUP_ERROR);
  const float upper_bound = x * (1 + SETUP_ERROR);

  // Keep running the filter until there were at least
  // `NUM_CONSECUTIVE_SETUP_SAMPLES` consecutive samples in the acceptable
  // range, or `MAX_SETUP_ITERS` samples (acting as a sort of timeout)
  uint8_t samples_in_range = 0;
  int i = 0;
  for (;
       samples_in_range <= NUM_CONSECUTIVE_SETUP_SAMPLES && i < MAX_SETUP_ITERS;
       i++) {
    result = run(x);
    if (lower_bound < result && result < upper_bound) {
      samples_in_range++;
    } else {
      samples_in_range = 0;
    }
  }
}

float IIRFilter::run(float x) {
  for (auto& filter : filters) {
    x = filter.run(x);
  }
  return x;
}

}  // namespace IIR
