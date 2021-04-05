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

#include <vector>

#include "iir.h"
#include "inference_core.h"
#include "intermediate_structures.h"

/**
 * @brief Smoothen the results of inference and average body parts since the
 * system assumes a side-on profile
 *
 */
namespace PostProcessing {

/**
 * @brief Process the output of an `Inference::InferenceCore`
 *
 * This class contains some processing steps to make the output of the
 * `Inference::InferenceCore` more useful. The following operations are
 * performed to process the raw data:
 * - Disregarding of any predictions below a given confidence threshold
 * - Smoothing location predictions in time
 * - Averaging location of left and right body parts if both exceed the
 * confidence threshold
 *
 */
class PostProcessor {
 private:
  float confidence_threshold;
  std::vector<IIR::IIRFilter> iir_filters;
  IIR::IIRFilter smoothed_trustworthiness;
  bool first_run = true;

 public:
  /**
   * @brief Construct a new Post Processor object
   *
   * @param confidence_threshold Confidence value that must be exceeded for a
   * prediction for a given body part to be useable
   * @param smoothing_settings Settings for the IIR filter
   */
  PostProcessor(float confidence_threshold,
                IIR::SmoothingSettings smoothing_settings);

  /**
   * @brief Apply post processing to the given `Inference::InferenceCore` output
   *
   * @param inference_core_output Output from an `Inference::InferenceCore`
   * being run
   * @return `ProcessedResults`
   */
  ProcessedResults run(Inference::InferenceResults inference_core_output);

  /**
   * @brief Dynamically update the confidence threshold to allow better
   * calibration
   *
   * @param confidence_threshold The updated confidence threshold in the range
   * [0, 1]
   * @return `true` If updating threshold was successful
   * @return `false` If the new threshold value is invalid
   */
  bool set_confidence_threshold(float confidence_threshold);

  /**
   * @brief Get the confidence threshold object
   *
   * @return `float` Currently set confidence threshold
   */
  float get_confidence_threshold();
};

}  // namespace PostProcessing
#endif  // SRC_POST_PROCESSOR_H_
