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

namespace PostProcessing {

PostProcessor::PostProcessor(float confidence_threshold,
                             IIR::SmoothingSettings smoothing_settings)
    : confidence_threshold(confidence_threshold) {
  // Initialise filters for x and y component of each body part position
  // It may be interesting to add a third filter for confidence in time
  for (int i = 0; i < (BodyPartMax + 1) * 2; i++) {
    this->iir_filters.push_back(IIR::IIRFilter(smoothing_settings));
  }
}

ProcessedResults PostProcessor::run(
    Inference::InferenceResults inference_core_output) {
  // Initialise structure for results
  ProcessedResults results;

  /* Go through all of the body parts and apply the two IIR filters to the
   * respective x and y components of the body part position. This results in
   * iterating through filters at twice the rate of iterating through body
   * parts; there are two filters per body part.*/

  int body_part_index = BodyPartMin;
  int filter_index = 0;
  Inference::Coordinate body_part;

  for (; body_part_index < BodyPartMax + 1;
       body_part_index++, filter_index += 2) {
    body_part = inference_core_output.body_parts.at(body_part_index);

    // Filter the incoming position for each body part
    // The incoming results structure is passed by value so it won't be
    // overwritten here
    body_part.x = this->iir_filters.at(filter_index).run(body_part.x);
    body_part.y = this->iir_filters.at(filter_index + 1).run(body_part.y);

    // Construct a `PostProcessing::Coordinate` with the filtered data
    results.body_parts.at(body_part_index) =
        Coordinate{body_part.x, body_part.y,
                   (body_part.confidence > this->confidence_threshold)
                       ? Status::Trustworthy
                       : Status::Untrustworthy};
  }

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

}  // namespace PostProcessing
