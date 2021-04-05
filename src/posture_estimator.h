/**
 * @file posture_estimator.h
 * @brief Interface for representation of user's pose
 *
 * @copyright Copyright (C) 2021  Conor Begley
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

#ifndef SRC_POSTURE_ESTIMATOR_H_
#define SRC_POSTURE_ESTIMATOR_H_

#include <stdio.h>

#include <string>

#include "intermediate_structures.h"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"

/**
 * @brief Responsible for analysing the results of pose estimation to determine
 * any posture changes which must be relayed to the user
 *
 */
namespace PostureEstimating {

/**
 * @brief A representation of a body part
 *
 * Each body part is represented as a `ConnectedJoint` to record the joints
 * relative position and the connected joints to this joint
 *
 * Angles are measured clockwise from Head
 *
 *      ^HEAD
 *      |
 *      |/ +30 degrees
 *      |
 *      |__ +90 degrees
 */

struct ConnectedJoint {
  Joint joint;
  PostProcessing::Coordinate coord;
  float upper_angle;
  float lower_angle;
};

/**
 * @brief Prints human readable string for enum `Joint`
 */
std::string stringJoint(Joint joint);

/**
 * @brief The representation of a human's pose, containing all
 * the expected `ConnectedJoint`
 */
struct Pose {
  /**
   * @brief Each element of this array corresponds to a body part with its
   * connections also specifed in a `PostureEstimating::ConnectedJoint`
   * structure
   *
   */
  std::array<ConnectedJoint, JointMax + 1> joints;
};

/**
 * @brief Creates an empty Pose object
 */
Pose createPose();

/**
 * @brief representation of user's pose for use by the pipeline processing
 */
struct PoseStatus {
  Pose ideal_pose;
  Pose current_pose;
  Pose pose_changes;
  bool good_posture;
  bool ideal_pose_set;
};

/**
 * @brief Colours corresponding to `PostureEstimator::colours`
 */
enum Colours { Red, Green, Blue };

/**
 * @brief This class handles representations of the user's pose and
 * calculates any updates to their pose that is required.
 *
 * It stores representations of the users current pose and their
 * calibrated ideal pose, from the `PostProcessing::ProcessedResult`.
 *
 * This class handles:
 * - Updating current user's pose or ideal user's pose.
 * - Calculating the angle between `ConnectedJoint`
 * - Comparing the current user's pose and ideal user's pose.
 *
 * General use should just consist of `runEstimator`.
 *
 */
class PostureEstimator {
 private:
  /**
   * @brief Array of colours used when indicating posture
   *
   */
  std::array<cv::Scalar, 3> colours = {
      cv::Scalar(255, 0, 0), cv::Scalar(0, 255, 0), cv::Scalar(0, 0, 255)};

  /**
   * @brief Calculates the angle(in degrees) between two points, clockwise
   * from the Head.
   *
   * @param coord1 Coordinate 1
   * @param coord2 Coordinate 2
   *
   * @return `float`
   */
  float getLineAngle(PostProcessing::Coordinate coord1,
                     PostProcessing::Coordinate coord2);

  /**
   * @brief Converts `PostProcessing::ProcessedResults` to a `Pose`.
   *
   * @param results The `PostProcessing::ProccessedResults` struct
   * from `PostProcessing::PostProcessor` being run.
   *
   * @return `Pose`
   */
  Pose createPoseFromResult(PostProcessing::ProcessedResults results);

  /**
   * @brief Update the user's pose based on the results from inference
   *
   * @param results `PostProcessing::ProcessedResults struct containing user's
   * pose data.
   */
  void update_current_pose(PostProcessing::ProcessedResults results);

  /**
   * @brief Compares user's current pose and ideal pose, returing a pose
   * object, with the angles of the `ConnectedJoint` representing the change
   * needed for each one to return current pose to ideal pose.
   *
   */
  void calculatePoseChanges();

  /**
   * @brief Checks the current pose_changes_needed and sees if they are still
   * within the good posture threshold. Designed to be used in conjunction with
   * calculate pose_changes. WARNING Does not calculate posture
   * changes.
   *
   */
  void checkGoodPosture();

  /**
   * @brief Calculates current posture and ideal posture difference and decides
   * if a good posture has been adopted based on threshold values.
   *
   */
  void calculateChangesAndCheckPosture();

  /**
   * @brief Updates the user's current pose from
   * `PostProcessing::ProcessingResults`, Calculates pose change needed. Checks
   * if pose change needed is outside threshold.
   *
   * @param results `PostProcessing::ProcessingResults` struct containing user's
   * pose data.
   * @return `true` if user's posture is good
   * @return `false` if user's posture is bad
   */
  bool updateCurrentPoseAndCheckPosture(
      PostProcessing::ProcessedResults results);

  /**
   * @brief Overlay the current pose for the current frame
   *
   * @param current_pose `PostureEstimating::Pose` The pose for the current
   * posture
   * @param current_frame `cv::Mat` Current frame to overlay lines onto
   * @param ideal_pose_set `true` If pose has been set in which case draw red
   * lines to indicate bad posture, and `false` if pose has not been set in
   * which case draw blue lines
   */
  void display_current_pose(PostureEstimating::Pose current_pose,
                            cv::Mat current_frame, bool ideal_pose_set);
  /**
   * @brief Overlay the pose changes for the current frame
   *
   * @param pose_changes `PostureEstimating::Pose` Changes needed to return to a
   * good posture
   * @param current_pose `PostureEstimating::Pose` The pose for the current
   * posture
   * @param current_frame `cv::Mat` Current frame to overlay lines onto
   */
  void display_pose_changes_needed(PostureEstimating::Pose pose_changes,
                                   PostureEstimating::Pose current_pose,
                                   cv::Mat current_frame);

 public:
  /**
   * @brief Construct a new `PostureEstimator` object
   */
  PostureEstimator();

  /**
   * @brief Destroy a `PostureEstimator` object
   */
  ~PostureEstimator() {}

  /**
   * @brief The user's current `Pose` from most recent data provided.
   */
  Pose current_pose;

  /**
   * @brief The user's ideal `Pose`, expected to be generated
   * during user calibration.
   */
  Pose ideal_pose;

  /**
   * @brief The `ideal_pose` is unset until the user manually sets it
   */
  bool ideal_pose_set = false;

  /**
   * @brief A representation of what changes are needed to get
   * back to ideal `Pose`.
   */
  Pose pose_changes;

  /**
   * @brief A representation of what absolute value changes for a pose
   * is acceptably still a good pose.
   */
  float pose_change_threshold;

  /**
   * @brief Whether the user is currently in a good posture (true) or if they
   * have adopted a poor posture (false).
   */
  bool good_posture;

  /**
   * @brief Calibrate the user's ideal pose using the results of
   * `PostProcessing`
   *
   * @param results `PostureEstimating::PoseStatus` struct containing user's
   * pose data.
   */
  void update_ideal_pose(PostureEstimating::Pose pose);

  /**
   * @brief Set the `pose_change_threshold` to allow user configuration. Note
   * that this threshold is in radians and we set a maximum configurable value
   * of 0.5 radians (28.64 degrees)
   *
   * @param threshold The updated threshold in the range [0..0.5] (radians)
   * @return `true` If updating threshold succeeded
   * @return `false` If updating threshold failed
   */
  bool set_pose_change_threshold(float threshold);

  /**
   * @brief Get the currently set `pose_change_threshold`. Note that this
   * threshold is in radians and we set a maximum configurable value of 0.5
   * radians (28.64 degrees)
   *
   * @return `float` The current threshold in the range [0..0.5] (radians)
   */
  float get_pose_change_threshold(void);

  /**
   * @brief Return a `PoseStatus` of the user's pose
   *
   * @param results `PostProcessing::ProcessedResults` struct containing user's
   * pose data.
   */
  PoseStatus runEstimator(PostProcessing::ProcessedResults results);

  /**
   * @brief Analyse the `PostureEstimating::PoseStatus` and use it as follows:
   *  - If `ideal_pose_set` is `false` then indicate this to user by
   * displaying the current pose as blue lines.
   * - If `ideal_pose_set` is `true` then check if  a `good_posture` is
   * detected.
   * - If `good_posture` is `true` then display current pose as green lines.
   * - If `good_posture` is `false` then use `pose_change_threshold` and
   * `pose_changes` to indicate which direction the user needs to move in order
   * to return to a good posture.
   *
   * @param pose_status `PostureEstimating::PoseStatus` The pose status for the
   * current frame
   * @param current_frame `cv::Mat` The current frame to overlay lines on to
   */
  void analysePosture(PostureEstimating::PoseStatus pose_status,
                      cv::Mat current_frame);
};
}  // namespace PostureEstimating
#endif  // SRC_POSTURE_ESTIMATOR_H_
