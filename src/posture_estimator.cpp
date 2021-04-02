/**
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

#include "posture_estimator.h"

#include <cmath>
#include <string>

#define MIN_POSE_CHANGE_THRESHOLD 0.0  ///< 0 degrees
#define MAX_POSE_CHANGE_THRESHOLD 0.5  ///< 28.64 degrees

namespace PostureEstimating {

std::string stringJoint(Joint joint) {
  switch (joint) {
    case Head:
      return "head";
    case Neck:
      return "neck";
    case Shoulder:
      return "shoulder";
    case Hip:
      return "hip";
    case Knee:
      return "knee";
    case Foot:
      return "foot";
    default:
      return "unknown";
  }
}

Pose createPose() {
  Pose p;

  for (int i = JointMin; i <= JointMax; i++) {
    p.joints[i] = {
        static_cast<Joint>(i), {0, 0, PostProcessing::Untrustworthy}, 0, 0};
  }
  return p;
}

PostureEstimator::PostureEstimator() {
  this->pose_change_threshold = 0.1;
  this->ideal_pose = createPose();
  this->current_pose = createPose();
  this->pose_changes = createPose();
}

float PostureEstimator::getLineAngle(PostProcessing::Coordinate coord1,
                                     PostProcessing::Coordinate coord2) {
  float x_dif = coord2.x - coord1.x;
  float y_dif = coord2.y - coord1.y;

  if (y_dif == 0) {
    return (x_dif > 0) ? M_PI / 2 : -M_PI / 2;
  }
  float slope = (x_dif / y_dif);
  if (y_dif > 0) {
    return atan(slope);
  } else {
    return (x_dif > 0) ? (M_PI + atan(slope)) : -(M_PI - atan(slope));
  }
}
Pose PostureEstimator::createPoseFromResult(
    PostProcessing::ProcessedResults results) {
  PostureEstimating::Pose p = createPose();

  for (int i = JointMin; i <= JointMax; i++) {
    p.joints[i].coord = results.body_parts[i];
  }

  p.joints[JointMin].lower_angle =
      getLineAngle(p.joints[JointMin].coord, p.joints[JointMin + 1].coord);

  for (int i = JointMin + 1; i < JointMax; i++) {
    p.joints[i].upper_angle =
        getLineAngle(p.joints[i].coord, p.joints[i - 1].coord);
    p.joints[i].lower_angle =
        getLineAngle(p.joints[i].coord, p.joints[i + 1].coord);
  }

  p.joints[JointMax].upper_angle =
      getLineAngle(p.joints[JointMax].coord, p.joints[JointMax - 1].coord);

  return p;
}

void PostureEstimator::update_current_pose(
    PostProcessing::ProcessedResults results) {
  this->current_pose = createPoseFromResult(results);
}

void PostureEstimator::calculatePoseChanges() {
  for (int i = JointMin; i <= JointMax; i++) {
    if (this->ideal_pose.joints[i].coord.status ==
            PostProcessing::Untrustworthy ||
        this->current_pose.joints[i].coord.status ==
            PostProcessing::Untrustworthy) {
      continue;
    } else {
      if (i > JointMin) {
        if (this->ideal_pose.joints[i - 1].coord.status ==
                PostProcessing::Trustworthy &&
            this->current_pose.joints[i - 1].coord.status ==
                PostProcessing::Trustworthy) {
          this->pose_changes.joints[i].upper_angle =
              this->ideal_pose.joints[i].upper_angle -
              this->current_pose.joints[i].upper_angle;
        }
      }
      if (i < JointMax) {
        if (this->ideal_pose.joints[i + 1].coord.status ==
                PostProcessing::Trustworthy &&
            this->current_pose.joints[i + 1].coord.status ==
                PostProcessing::Trustworthy) {
          this->pose_changes.joints[i].lower_angle =
              this->ideal_pose.joints[i].lower_angle -
              this->current_pose.joints[i].lower_angle;
        }
      }
    }
  }
}

void PostureEstimator::checkGoodPosture() {
  for (int i = JointMin + 1; i <= JointMax - 2; i++) {
    if ((fabs(this->pose_changes.joints[i].upper_angle) <
         -(this->pose_change_threshold)) ||
        fabs(this->pose_changes.joints[i].upper_angle) >
            this->pose_change_threshold) {
      this->good_posture = false;
      return;
    }
  }
  this->good_posture = true;
}

void PostureEstimator::calculateChangesAndCheckPosture() {
  calculatePoseChanges();
  checkGoodPosture();
}

bool PostureEstimator::updateCurrentPoseAndCheckPosture(
    PostProcessing::ProcessedResults results) {
  update_current_pose(results);
  calculateChangesAndCheckPosture();
  return this->good_posture;
}

void PostureEstimator::update_ideal_pose(PostureEstimating::Pose pose) {
  this->ideal_pose_set = true;
  this->ideal_pose = pose;
}

bool PostureEstimator::set_pose_change_threshold(float threshold) {
  if (MIN_POSE_CHANGE_THRESHOLD <= threshold &&
      threshold <= MAX_POSE_CHANGE_THRESHOLD) {
    this->pose_change_threshold = threshold;
    return true;
  }
  return false;
}

PoseStatus PostureEstimator::runEstimator(
    PostProcessing::ProcessedResults results) {
  this->updateCurrentPoseAndCheckPosture(results);
  PoseStatus p = {this->ideal_pose, this->current_pose, this->pose_changes,
                  this->good_posture, this->ideal_pose_set};
  return p;
}  // namespace PostureEstimating

}  // namespace PostureEstimating
