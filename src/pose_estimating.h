/*
 * @file pose.h
 * @brief Interface for representation of user's pose

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

#ifndef SRC_POSE_ESTIMATING_H_
#define SRC_POSE_ESTIMATING_H_

#include "post_processor.h"

namespace PoseEstimating {

/**
 * @brief An enum for each important pose body part
 */
enum Joint {
  Head,
  Neck,
  Shoulder,
  Hip,
  Knee,
  Foot
}

/*
 * @brief A representation of a body part
 *
 * Each body part is represented as a `Pose::ConnectedJoint` to record the joints relative position and the connected joints to this joint
 * 
 */
struct ConnectedJoint {
  Joint joint;
  /* @brief relative x co-ordinate of joint */
  PostProcessing::Coordindate coord;
  ConnectedJoint * upper_connected_joint;
  /* 
   * @brief Angle(in degrees) of upper joint measured anticlockwise from bottom left corner  
   */
  float upper_angle;
  ConnectedJoint * lower_connected_joint;
  /* 
   * @brief Angle(in degrees) of lower joint measured anticlockwise from bottom left corner  
   */
  float lower_angle;
}

/* 
 *  @brief The `Pose::Pose` representation of a human's pose, containing all the expected `Pose::ConnectedJoint 
 */
struct Pose {
  ConnectedJoint head;
  ConnectedJoint neck;
  ConnectedJoint shoulder;
  ConnectedJoint hip;
  ConnectedJoint knee;
  ConnectedJoint foot;
}

/*
 * @brief This class handles representations of the user's pose and calculates any updates to their pose that is required.
 * 
 * This class stores representations of the users current pose and their calibrated ideal pose, from the results of the PostProcessor class.
 *
 * This class handles:
 * Updating current user's pose or ideal user's pose
 * Calculating the angle between ConnectedJoints
 * Comparing the current user's pose and ideal user's pose
 *
 * General use should just consist of updateAndCheckCurrentPose()
 * 
 */
class PoseEstimater {
 private:
  /*
   * @brief Calculates the angle(in degrees) between two points, anticlockwise from origin / bottom left corner of image
   * @param x1 Relative X co-ordinate of first point
   * @param y1 Relative Y co-ordinate of second point
   * @param x2 Relative X co-ordinate of first point
   * @param y2 Relative Y co-ordinate of first point
   *
   * @returns float
   */
  float getLineAngle(float x1, float y1, float x2, float y2)

  /*
   * @brief Converts `PostProcesser::ProcessedResults` to a `Pose::Pose`
   *
   * @param results  The PostProcessor::ProccessedResults struct from PostProcessor::PostProcessor being run
   * @return JointsPose
   */
  Pose createPose(PostProcessing :: ProcessedResults results);

  /*
   * @brief sets user's current pose
   *
   * @param Pose
   */
  void set_current_pose(Pose current);

  /*
   * @brief sets user's ideal pose
   *
   * @param Pose
   */
  void set_idea_pose(Pose current);

  /*
   * @brief sets calcaluted changes needed pose
   *
   * @param Pose
   */
  void set_pose_changes_needed(Pose current);

 public:
  /*
   * @brief The user's current pose from most recent data provided
   */
  Pose current_pose;

  /* 
   * @brief The user's ideal pose, expected to be generated during user calibration  
   */
  Pose ideal_pose;

  /*
   * @brief A representation of what changes are needed to get back to ideal pose
   */
  Pose pose_changes_needed;

  /*
   * @brief A representation of what absolute value changes for a pose is acceptably still a good pose
   */
  float pose_change_threshold;

  /*
   * @brief Whether the user is in a good pose (true) or bad pose (false)
   */
  bool good_pose;

  /*
   * @brief Returns the user's current pose
   * 
   * @returns Pose
   */
  Pose get_current_pose();

  /*
   * @brief Updates the user's current pose from PostProcesser results
   *
   * @param results ProcessedResults struct containing user's pose data
   */
  void update_current_pose(PostProcessing::ProcessedResults results);

  /*
   * @brief Returns the user's ideal pose
   * 
   * @returns Pose
   */
  Pose get_ideal_pose();

  /*
   * @brief Updates the user's ideal pose from PostProcesser results
   *
   * @param results ProcessedResults struct containing user's pose data
   */
  void update_ideal_pose(PostProcessing::ProcessedResults results);

  /*
   * @brief Gets the current threshold for deciding if a ConnectJoint angle is considered good posture relative to ideal posture 
   */
  float get_pose_change_threshold();

  /*
   * @brief Sets the current threshold for deciding if a ConnectJoint angle is considered good posture relative to ideal posture 
   */
  void set_pose_change_threshold(int threshold);

  /*
   * @brief Returns last calculated users pose, does NOT calculate current pose differnece
   * This is intended to be used in conjuction with updateCurrentPoseCheck
   * When updateCurrentPoseCheck returns false, this funtion should be used to retrieve the necessary changes
   */   
  Pose get_pose_changes_needed();

  /*
   * @brief Compares user's current pose and ideal pose, returing a pose object, with the angles of the ConnectJoints representing the change needed for each 
   * one to return current pose to ideal pose. 
   *
   * @returns Pose
   */  
  Pose calculate_pose_changes_needed();

  /*
   * @brief Checks the current pose_changes_needed and sees if they are still withing the good posture threshold
   * Designed to be used in conjunction with calculate_pose_changes_needed
   */
  bool check_good_posture();

  /*
   * @brief Updates the user's current pose from PostProcesser results, 
   * Calculates pose change needed 
   * Checks if pose change needed is outside threshold
   * Returns true or false if user in bad pose
   * 
   * @param results ProcessedResults struct containing user's pose data
   * 
   * @returns bool
   */
  bool updateCurrentPoseAndCheck(PostProcessing::ProcessedResults results);
}
}  // namespace PoseEstimating
#endif  // SRC_POSE_ESTIMATING_H_

