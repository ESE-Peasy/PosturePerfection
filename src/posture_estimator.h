/**
 * @file posture_estimator.h
 * @brief Interface for representation of user's pose
 *
 * @copyright Copyright (C) 2021 Conor Begley
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef SRC_POSTURE_ESTIMATOR_H_
#define SRC_POSTURE_ESTIMATOR_H_

#include <stdio.h>

#include <string>

#include "intermediate_structures.h"
namespace PostureEstimating {

/**
 * @brief A representation of a body part
 *
 * Each body part is represented as a `ConnectedJoint` to record the joints
 * relative position and the connected joints to this joint
 *
 * Angles are measured clockwise from Head
 *
 * ^HEAD
 * |
 * |/ +30 degrees
 * |
 * |__ +90 degrees
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
std::string stringJoint(Joint j);

/**
 * @brief The representation of a human's pose, containing all
 * the expected `ConnectedJoint`
 */
struct Pose {
  std::array<ConnectedJoint*, JointMax + 1> joints;
};

/**
 * @brief Creates an empty Pose object
 */
Pose createPose();

/**
 * @brief Frees all pointers in a Pose object
 *
 * @param `Pose`
 */

void destroyPose(PostureEstimating::Pose p);

/**
 * @brief representation of user's pose for use by the pipeline processing
 */
struct PoseStatus {
  Pose ideal_pose;
  Pose current_pose;
  Pose pose_changes;
  bool bool_good_posture;
};

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
 * General use should just consist of `updateCurrentPoseAndCheckPosture`.
 *
 */
class PostureEstimator {
 private:
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

 public:
  /**
   * @brief Construct a new Posture Estimator object
   */
  PostureEstimator();

  /**
   * @brief Destroy a Posture Estimator object
   */
  ~PostureEstimator();

  /**
   * @brief The user's current pose from most recent data provided.
   */
  Pose current_pose;

  /**
   * @brief The user's ideal pose, expected to be generated
   * during user calibration.
   */
  Pose ideal_pose;

  /**
   * @brief A representation of what changes are needed to get
   * back to ideal pose.
   */
  Pose pose_changes;

  /**
   * @brief A representation of what absolute value changes for a pose
   * is acceptably still a good pose.
   */
  float pose_change_threshold;

  /**
   * @brief Whether the user is in a good pose (true) or bad pose (false).
   */
  bool good_posture;

  /**
   * @brief Calibrate the user's ideal pose using
   *
   * @param results `PostProcessing::ProcessedResults` struct containing user's
   * pose data.
   */
  void update_ideal_pose(PostProcessing::ProcessedResults results);

  /**
   * @brief Return a `PoseStatus` of the user's pose
   *
   * @param results `PostProcessing::ProcessedResults` struct containing user's
   * pose data.
   */
  PoseStatus runEstimator(PostProcessing::ProcessedResults results);
};
}  // namespace PostureEstimating
#endif  // SRC_POSTURE_ESTIMATOR_H_
