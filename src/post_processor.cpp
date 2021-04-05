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

#include "post_processor.h"

#include "iir.h"
#include "intermediate_structures.h"

#define MIN_CONF_THRESH 0.0  // Minimum value a confidence threshold can be
#define MAX_CONF_THRESH 1.0  // Maximum value a confidence threshold can be
#define NUM_FILTERS_PER_BODY_PART 3  // No. IIR filters for each body part

/**
 * @brief Take the mean of two body parts
 *
 * Intended for use with two body parts that lie left and right of the spine
 * (as an axis of symmetry). As the system is for use with the person in
 * profile, these should overlap, but there may be slight differences.
 *
 * If both input `PostProcessing::Coordinate`s are
 * `PostProcessing::Trustworthy` the arithmetic mean of the two is returned (x
 * and y coordinates are averaged independently). If only one body part is
 * `PostProcessing::Trustworthy` only that body part is returned, i.e., no
 * averaging takes place. In the case where both are
 * `PostProcessing::Untrustworthy`, the mean is returned, although this may
 * not be particularly meaningful as indicated by the `Untrustworthy` flag.
 *
 * @param left a `Inference::Coordinate`
 * @param right a `Inference::Coordinate`
 * @return `PostProcessing::Coordinate` the "intelligent" mean
 */
PostProcessing::Coordinate intelligent_mean_body_parts(
    PostProcessing::Coordinate left, PostProcessing::Coordinate right) {
  if (left.status == PostProcessing::Trustworthy &&
      right.status == PostProcessing::Untrustworthy) {
    return left;
  } else if (left.status == PostProcessing::Untrustworthy &&
             right.status == PostProcessing::Trustworthy) {
    return right;
  } else {
    // Both Trustworthy or Untrustworthy
    float x = (left.x + right.x) / 2;
    float y = (left.y + right.y) / 2;
    return PostProcessing::Coordinate{x, y, left.status};
  }
}

namespace PostProcessing {

PostProcessor::PostProcessor(float confidence_threshold,
                             IIR::SmoothingSettings smoothing_settings)
    : confidence_threshold(confidence_threshold),
      smoothed_trustworthiness(smoothing_settings) {
  // Initialise filters for x and y component of each body part position
  // It may be interesting to add a third filter for confidence in time
  for (int i = 0; i < (BodyPartMax + 1) * NUM_FILTERS_PER_BODY_PART; i++) {
    this->iir_filters.push_back(IIR::IIRFilter(smoothing_settings));
  }
}

ProcessedResults PostProcessor::run(
    Inference::InferenceResults inference_core_output) {
  // Set the filters up for the very first frame
  if (first_run) {
    first_run = false;
    int body_part_index = BodyPartMin;
    int filter_index = 0;
    Inference::Coordinate body_part;

    for (; body_part_index < BodyPartMax + 1;
         body_part_index++, filter_index += NUM_FILTERS_PER_BODY_PART) {
      body_part = inference_core_output.body_parts.at(body_part_index);

      iir_filters.at(filter_index).set(body_part.x);
      iir_filters.at(filter_index + 1).set(body_part.y);

      iir_filters.at(filter_index + 2).set(body_part.confidence);
    }
  }

  // Initialise structure for results
  std::array<Coordinate, BodyPartMax + 1> intermediate_results;
  ProcessedResults results;

  /* Go through all of the body parts and apply the two IIR filters to the
   * respective x and y components of the body part position. This results in
   * iterating through filters at twice the rate of iterating through body
   * parts; there are two filters per body part.*/

  int body_part_index = BodyPartMin;
  int filter_index = 0;
  Inference::Coordinate body_part;
  Coordinate processed_body_part;

  for (; body_part_index < BodyPartMax + 1;
       body_part_index++, filter_index += NUM_FILTERS_PER_BODY_PART) {
    body_part = inference_core_output.body_parts.at(body_part_index);

    // Filter the incoming position for each body part
    // The incoming results structure is passed by value so it won't be
    // overwritten here
    body_part.x = iir_filters.at(filter_index).run(body_part.x);
    body_part.y = iir_filters.at(filter_index + 1).run(body_part.y);
    body_part.confidence =
        iir_filters.at(filter_index + 2).run(body_part.confidence);

    processed_body_part =
        Coordinate{body_part.x, body_part.y,
                   (body_part.confidence > this->confidence_threshold)
                       ? Status::Trustworthy
                       : Status::Untrustworthy};
    intermediate_results.at(body_part_index) = processed_body_part;
  }

  // Assemble results with the filtered data and any necessary averaging
  // Head
  results.body_parts.at(Head) = intermediate_results.at(head_top);

  // Neck
  results.body_parts.at(Neck) = intermediate_results.at(upper_neck);

  // Shoulder
  Coordinate mean_shoulder =
      intelligent_mean_body_parts(intermediate_results.at(left_shoulder),
                                  intermediate_results.at(right_shoulder));
  results.body_parts.at(Shoulder) = mean_shoulder;

  // Hip (from left/right hip and pelvis)
  Coordinate mean_hip = intelligent_mean_body_parts(
      intermediate_results.at(left_hip), intermediate_results.at(right_hip));
  mean_hip =
      intelligent_mean_body_parts(mean_hip, intermediate_results.at(pelvis));
  results.body_parts.at(Hip) = mean_hip;

  // Knee
  Coordinate mean_knee = intelligent_mean_body_parts(
      intermediate_results.at(left_knee), intermediate_results.at(right_knee));
  results.body_parts.at(Knee) = mean_knee;

  // Foot (from ankles)
  Coordinate mean_ankle =
      intelligent_mean_body_parts(intermediate_results.at(left_ankle),
                                  intermediate_results.at(right_ankle));
  results.body_parts.at(Foot) = mean_ankle;

  return results;
}

bool PostProcessor::set_confidence_threshold(float confidence_threshold) {
  if (MIN_CONF_THRESH <= confidence_threshold &&
      confidence_threshold <= MAX_CONF_THRESH) {
    this->confidence_threshold = confidence_threshold;
    return true;
  } else {
    return false;
  }
}

float PostProcessor::get_confidence_threshold() { return confidence_threshold; }

}  // namespace PostProcessing
