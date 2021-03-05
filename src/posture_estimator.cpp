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

#include "posture_estimator.h"

#include <cmath>

namespace PostureEstimating {

std::string stringJoint(Joint j) {
  switch (j) {
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
    p.joints[i] =
        reinterpret_cast<ConnectedJoint *>(malloc(sizeof(ConnectedJoint)));
    p.joints[i]->joint = static_cast<Joint>(i);
    p.joints[i]->coord = {0, 0, PostProcessing::Untrustworthy};
    p.joints[i]->upper_angle = 0;
    p.joints[i]->lower_angle = 0;
  }

  p.joints[JointMin]->upper_connected_joint = nullptr;
  p.joints[JointMin]->lower_connected_joint = p.joints[JointMin + 1];

  for (int i = JointMin + 1; i <= JointMax; i++) {
    p.joints[i]->upper_connected_joint = p.joints[i - 1];
    p.joints[i]->lower_connected_joint = p.joints[i + 1];
  }

  p.joints[JointMax]->upper_connected_joint = p.joints[JointMax - 1];
  p.joints[JointMax]->lower_connected_joint = nullptr;

  return p;
}

void destroyPose(Pose p) {
  for (int i = JointMin; i < JointMax; i++) {
    free(p.joints[i]);
  }
}

PostureEstimator::PostureEstimator() {
  this->pose_change_threshold = 0;
  this->ideal_pose = createPose();
  this->current_pose = createPose();
  this->pose_changes = createPose();
}

PostureEstimator::~PostureEstimator() {
  destroyPose(this->ideal_pose);
  destroyPose(this->current_pose);
  destroyPose(this->pose_changes);
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
    p.joints[i]->coord = results.body_parts[i];
  }

  p.joints[JointMin]->lower_angle =
      getLineAngle(p.joints[JointMin]->coord, p.joints[JointMin + 1]->coord);

  for (int i = JointMin + 1; i < JointMax; i++) {
    p.joints[i]->upper_angle =
        getLineAngle(p.joints[i]->coord, p.joints[i - 1]->coord);
    p.joints[i]->lower_angle =
        getLineAngle(p.joints[i]->coord, p.joints[i + 1]->coord);
  }

  p.joints[JointMax]->upper_angle =
      getLineAngle(p.joints[JointMax]->coord, p.joints[JointMax - 1]->coord);

  return p;
}

void PostureEstimator::update_current_pose(
    PostProcessing::ProcessedResults results) {
  destroyPose(this->current_pose);
  this->current_pose = createPoseFromResult(results);
}

void PostureEstimator::calculatePoseChanges() {
  for (int i = JointMin; i <= JointMax; i++) {
    if (this->ideal_pose.joints[i]->coord.status ==
            PostProcessing::Untrustworthy ||
        this->current_pose.joints[i]->coord.status ==
            PostProcessing::Untrustworthy) {
      continue;
    } else {
      if (this->ideal_pose.joints[i]->upper_connected_joint != nullptr &&
          this->current_pose.joints[i]->upper_connected_joint != nullptr) {
        this->pose_changes.joints[i]->upper_angle =
            this->ideal_pose.joints[i]->upper_angle -
            this->current_pose.joints[i]->upper_angle;
      }
      if (this->ideal_pose.joints[i]->lower_connected_joint != nullptr &&
          this->current_pose.joints[i]->lower_connected_joint != nullptr) {
        this->pose_changes.joints[i]->lower_angle =
            this->ideal_pose.joints[i]->lower_angle -
            this->current_pose.joints[i]->lower_angle;
      }
    }
  }
}

void PostureEstimator::checkGoodPosture() {
  for (int i = JointMin; i <= JointMax; i++) {
    if ((fabs(this->pose_changes.joints[i]->lower_angle) >
         this->pose_change_threshold) ||
        fabs(this->pose_changes.joints[i]->upper_angle) >
            this->pose_change_threshold) {
      this->good_posture = false;
      return;
    }
  }
  this->good_posture = true;
}

void PostureEstimator::update_ideal_pose(
    PostProcessing::ProcessedResults results) {
  destroyPose(this->ideal_pose);
  this->ideal_pose = createPoseFromResult(results);
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

}  // namespace PostureEstimating
