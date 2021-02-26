/**
 * @file intermediate_structures.h
 * @brief Data structures used in communicating between different parts of the
 * system
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

#ifndef SRC_INTERMEDIATE_STRUCTURES_H_
#define SRC_INTERMEDIATE_STRUCTURES_H_

#include <array>

/**
 * @brief Defines the order of body parts in the `body_parts` arrays
 *
 * The `Inference::InferenceResults` and `PostProcessing::ProcessedResults` each
 * have a `body_parts` field. The order of body parts is defined by this enum.
 * The minimum and maximum body part "value" is given by `BodyPartMin` and
 * `BodyPartMax`, respectively.
 *
 */
enum BodyPart {
  head_top,
  BodyPartMin = head_top,
  upper_neck,
  right_shoulder,
  right_elbow,
  right_wrist,
  thorax,
  left_shoulder,
  left_elbow,
  left_wrist,
  pelvis,
  right_hip,
  right_knee,
  right_ankle,
  left_hip,
  left_knee,
  left_ankle,
  BodyPartMax = left_ankle
};

namespace PostProcessing {

/**
 * @brief A `PostProcessing::Coordinate`'s position can be either
 * `Untrustworthy` or `Trustworthy`
 *
 * This is derived from the confidence in predictions from the model.
 *
 */
enum Status { Untrustworthy, Trustworthy };

/**
 * @brief Indicates position and trustworthyness of a body part node
 *
 */
struct Coordinate {
  float x;
  float y;
  Status status;
};

/**
 * @brief The `PostProcessing::Coordinate` of each body part
 *
 * Each body part is expressed as a `PostProcessing::Coordinate` to indicate
 * the best estimate for that body part's location in the current frame. This
 * is not necessarily the same as the output of the pose estimation model for
 * a given frame.
 *
 */
struct ProcessedResults {
  std::array<Coordinate, BodyPartMax + 1> body_parts;
};
}  // namespace PostProcessing

namespace Inference {

/**
 * @brief Indicates position and confidence in positioning of a body part node
 *
 */
struct Coordinate {
  float x;
  float y;
  float confidence;
};

/**
 * @brief Results of running the model for all body parts
 *
 */
struct InferenceResults {
  std::array<Coordinate, BodyPartMax + 1> body_parts;
};
}  // namespace Inference

#endif  // SRC_INTERMEDIATE_STRUCTURES_H_
