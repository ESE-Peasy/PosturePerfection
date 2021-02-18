/**
 * @file post_processor.h
 * @brief Interface for post processing the output of an
 * `Inference::InferenceCore`
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

#ifndef SRC_POST_PROCESSOR_H_
#define SRC_POST_PROCESSOR_H_

#include "inference_core.h"

namespace PostProcessing {

enum Status { Untrustworthy Trustworthy };

typedef struct Coordinate {
  float x;
  float y;
  Status status;
} Coordinate;

/**
 * @brief The `PostProcessing::Coordinate` of each body part
 *
 * Each body part is expressed as a `PostProcessing::Coordinate` to indicate the best
 * estimate for that body part's location in the current frame. This is not
 * necessarily the same as the output of the pose estimation model for a given
 * frame.
 *
 */
typedef struct ProcessedResults {
  Coordinate head_top;
  Coordinate upper_neck;
  Coordinate right_shoulder;
  Coordinate right_elbow;
  Coordinate right_wrist;
  Coordinate thorax;
  Coordinate left_shoulder;
  Coordinate left_elbow;
  Coordinate left_wrist;
  Coordinate pelvis;
  Coordinate right_hip;
  Coordinate right_knee;
  Coordinate right_ankle;
  Coordinate left_hip;
  Coordinate left_knee;
  Coordinate left_ankle;
} ProcessedResults;

typedef struct SmoothingSettings {
} SmoothingSettings;

/**
 * @brief Process the output of an `Inference::InferenceCore`
 *
 * This class contains some processing steps to make the output of the
 * `Inference::InferenceCore` more useful. The following operations are
 * performed to process the raw data:
 * - Disregarding of any predictions below a given confidence threshold
 * - Smoothing location predictions in time
 *
 */
class PostProcessor {
 public:
  /**
   * @brief Construct a new Post Processor object
   *
   * @param confidence_threshold Confidence value that must be exceeded for a
   * prediction for a given body part to be useable
   * @param smoothing_settings
   */
  PostProcessor(float confidence_threshold,
                SmoothingSettings smoothing_settings);

  /**
   * @brief Apply post processing to the given `Inference::InferenceCore` output
   *
   * @param inference_core_output Output from an `Inference::InferenceCore`
   * being run
   * @return ProcessedResults
   */
  ProcessedResults run(Inference::InferenceResults inference_core_output);

  /**
   * @brief Set the confidence threshold
   *
   * @param confidence_threshold The updated confidence threshold in the range
   * [0, 1]
   * @return true If updating threshold was successful
   * @return false If the new threshold value is invalid
   */
  bool set_confidence_threshold(float confidence_threshold);
}

}  // namespace PostProcessing
#endif  // SRC_POST_PROCESSOR_H_
