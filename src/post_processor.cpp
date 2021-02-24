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

#define MIN_CONF_THRESH 0.0 // Minimum value a confidence threshold can be
#define MAX_CONF_THRESH 1.0 // Maximum value a confidence threshold can be



namespace PostProcessing {

PostProcessor::PostProcessor(float confidence_threshold, SmoothingSettings smoothing_settings) : confidence_threshold(confidence_threshold), smoothing_settings(smoothing_settings) {}

ProcessedResults PostProcessor::run(InferenceResults inference_core_output) {
  //
}

PostProcessor::set_confidence_threshold(float confidence_threshold) {
  if (MIN_CONF_THRESH <= confidence_threshold && confidence_threshold <= MAX_CONF_THRESH) {
    this->confidence_threshold = confidence_threshold;
    return true;
  } else {
    return false;
  }
}

}  // namespace PostProcessing
