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
  for (int i = 0; i < (BodyPartMax + 1) * 2; i++) {
    this->iir_filters.push_back(IIR::IIRFilter(smoothing_settings));
  }
}

ProcessedResults PostProcessor::run(
    Inference::InferenceResults inference_core_output) {
  ProcessedResults results;

  // Convert to `Inference::Coordinate`
  int body_part_index = BodyPartMin;
  for (auto body_part : inference_core_output.body_parts) {
    results.body_parts[body_part_index] =
        Coordinate{body_part.x, body_part.y,
                   (body_part.confidence > this->confidence_threshold)
                       ? Status::Trustworthy
                       : Status::Untrustworthy};
    body_part_index++;
  }

  /* Go through all of the body parts and apply the two IIR filters to the
   * respective x and y components of the body part position. This results in
   * iterating through filters at twice the rate of iterating through body
   * parts; there are two filters per body part.*/
  body_part_index = BodyPartMin;
  for (auto& body_part : results.body_parts) {
    body_part.x = this->iir_filters.at(body_part_index).run(body_part.x);
    body_part.y = this->iir_filters.at(body_part_index + 1).run(body_part.y);
    body_part_index += 2;
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
