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

#include "iir/iir.h"

#define MIN_CONF_THRESH 0.0  // Minimum value a confidence threshold can be
#define MAX_CONF_THRESH 1.0  // Maximum value a confidence threshold can be

namespace PostProcessing {

PostProcessor::PostProcessor(float confidence_threshold,
                             SmoothingSettings smoothing_settings)
    : confidence_threshold(confidence_threshold),
      smoothing_settings(smoothing_settings) {
  this->iir_filter = IIR::IIRFilter(smoothing_settings);
}

Coordinate PostProcessor::inference_results_processed_results(
    Inference::Coordinate coord_in) {
  return Coordinate{coord_in.x, coord_in.y,
                    (coord_in.confidence > this->confidence_threshold)
                        ? Status::Trustworthy
                        : Status::Untrustworthy};
}

ProcessedResults PostProcessor::run(
    Inference::InferenceResults inference_core_output) {
  ProcessedResults results;
  results.head_top =
      inference_results_processed_results(inference_core_output.head_top);
  results.left_ankle =
      inference_results_processed_results(inference_core_output.left_ankle);
  results.left_elbow =
      inference_results_processed_results(inference_core_output.left_elbow);
  results.left_hip =
      inference_results_processed_results(inference_core_output.left_hip);
  results.left_knee =
      inference_results_processed_results(inference_core_output.left_knee);
  results.left_shoulder =
      inference_results_processed_results(inference_core_output.left_shoulder);
  results.left_wrist =
      inference_results_processed_results(inference_core_output.left_wrist);
  results.pelvis =
      inference_results_processed_results(inference_core_output.pelvis);
  results.right_ankle =
      inference_results_processed_results(inference_core_output.right_ankle);
  results.right_elbow =
      inference_results_processed_results(inference_core_output.right_elbow);
  results.right_hip =
      inference_results_processed_results(inference_core_output.right_hip);
  results.right_knee =
      inference_results_processed_results(inference_core_output.right_knee);
  results.right_shoulder =
      inference_results_processed_results(inference_core_output.right_shoulder);
  results.right_wrist =
      inference_results_processed_results(inference_core_output.right_wrist);
  results.thorax =
      inference_results_processed_results(inference_core_output.thorax);
  results.upper_neck =
      inference_results_processed_results(inference_core_output.upper_neck);

  return results;
}

PostProcessor::set_confidence_threshold(float confidence_threshold) {
  if (MIN_CONF_THRESH <= confidence_threshold &&
      confidence_threshold <= MAX_CONF_THRESH) {
    this->confidence_threshold = confidence_threshold;
    return true;
  } else {
    return false;
  }
}

}  // namespace PostProcessing
