/**
 * @file posture_estimator.h
 * @brief Interface for representation of user's pose
 *
 * @copyright Copyright (C) 2021  Conor Begley
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

#include "posture_estimator.h"

#include <stdio.h>

#include <cmath>

namespace PostureEstimating {
PostureEstimator::PostureEstimator() {}

float PostureEstimator::getLineAngle(PostProcessing::Coordinate coord1,
                                     PostProcessing::Coordinate coord2) {
  float x_dif = coord2.x - coord1.x;
  float y_dif = coord2.y - coord1.y;
  if (x_dif == 0 || y_dif == 0) {
    return M_PI / 2;
  }
  float slope = y_dif / x_dif;
  return M_PI / 2 - atan(slope);
}
}  // namespace PostureEstimating
