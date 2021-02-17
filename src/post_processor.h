/**
 * @file post_processor.h
 * @brief Interface for post processing the output of an
 * `Inference::InferenceCore`
 *
 */

#ifndef SRC_POST_PROCESSOR_H_
#define SRC_POST_PROCESSOR_H_

#include "inference_core.h"

namespace PostProcessing {

/**
 * @brief The `Inference::Coordinate` of each body part
 *
 * Each body part is expressed as a `Inference::Coordinate` to indicate the best
 * estimate for that body part's location in the current frame. This is not
 * necessarily the same as the output of the pose estimation model for a given
 * frame.
 *
 */
typedef struct ProcessedResults {
  Inference::Coordinate head_top;
  Inference::Coordinate upper_neck;
  Inference::Coordinate right_shoulder;
  Inference::Coordinate right_elbow;
  Inference::Coordinate right_wrist;
  Inference::Coordinate thorax;
  Inference::Coordinate left_shoulder;
  Inference::Coordinate left_elbow;
  Inference::Coordinate left_wrist;
  Inference::Coordinate pelvis;
  Inference::Coordinate right_hip;
  Inference::Coordinate right_knee;
  Inference::Coordinate right_ankle;
  Inference::Coordinate left_hip;
  Inference::Coordinate left_knee;
  Inference::Coordinate left_ankle;
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
  ProcessedResults run(InferenceResults inference_core_output);

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
