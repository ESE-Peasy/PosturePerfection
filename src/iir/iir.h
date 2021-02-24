/**
 * @file iir.h
 * @brief Very simple IIR filter
 *
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
#ifndef SRC_IIR_IIR_H_
#define SRC_IIR_IIR_H_

#include "../post_processor.h"

namespace IIR {

/**
 * @brief Taps inside the second-order IIR filter
 *
 */
typedef Nodes struct {
  float b0;
  float b1;
  float b2;
  float a0;
  float a1;
  float a2;
  float tap1;
  float tap2;
} Nodes;

class IIR2ndOrderFilter {
 private:
  Nodes nodes;

 public:
  IIR2ndOrderFilter(float* coefficients);
  run(float x);
};

class IIRFilter {
 private:
  /**
   * @brief Array of the constituent second-order filters
   *
   */
  IIR2ndOrderFilter* filters;
  size_t num_stages;

 public:
  IIRFilter(PostProcessing::ProcessedResults smoothing_settings);
  ~IIRFilter();
  run(float x);
};

}  // namespace IIR
#endif  // SRC_IIR_IIR_H_
