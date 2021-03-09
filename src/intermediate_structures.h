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
#include <string>

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

/**
 * @brief Defines order of joints in any body part arrays
 *
 * This is to be used where the pose is represented as a profile view to define
 * the posture. As a result there are no left and right versions
 *
 */
enum Joint {
  Head = 0,
  JointMin = Head,
  Neck = 1,
  Shoulder = 2,
  Hip = 3,
  Knee = 4,
  Foot = 5,
  JointMax = Foot
};

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
}  // namespace PreProcessing

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
 * @brief Converts the Untrustworthy/Trustworthy status to a string.
 *
 */
std::string stringTrustworthy(Status s);

/**
 * @brief Indicates position and trustworthiness of a body part node
 *
 * This type of `PostProcessing::Coordinate` is to be used after post processing
 * has been applied as it contains a generalised indication of how "useable" the
 * prediction is. This abstracts away the specific underlying model that was
 * used to generate the results and should be seen as a better representation of
 * the user's current pose than a `Inference::Coordinate`.
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
 * Iterate over this using the `Joint` enum
 *
 */
struct ProcessedResults {
  std::array<Coordinate, JointMax + 1> body_parts;
};
}  // namespace PostProcessing

namespace Inference {

/**
 * @brief Indicates position and confidence in positioning of a body part node
 *
 * This type of `Inference::Coordinate` is to be used to represent the raw
 * output of the pose estimation model. Results using this type of coordinate
 * are tightly bound to the specific model in use as they contain the raw
 * confidence value for the point's predicted location.
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
