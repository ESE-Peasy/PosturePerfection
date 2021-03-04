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

#include <stdio.h>

#include <cmath>

namespace PostureEstimating {

Pose createPose() {
  Pose p;
  p.joints[Head] =
      reinterpret_cast<ConnectedJoint *>(malloc(sizeof(ConnectedJoint)));
  p.joints[Neck] =
      reinterpret_cast<ConnectedJoint *>(malloc(sizeof(ConnectedJoint)));
  p.joints[Shoulder] =
      reinterpret_cast<ConnectedJoint *>(malloc(sizeof(ConnectedJoint)));
  p.joints[Hip] =
      reinterpret_cast<ConnectedJoint *>(malloc(sizeof(ConnectedJoint)));

  p.joints[Knee] =
      reinterpret_cast<ConnectedJoint *>(malloc(sizeof(ConnectedJoint)));
  p.joints[Foot] =
      reinterpret_cast<ConnectedJoint *>(malloc(sizeof(ConnectedJoint)));

  p.joints[Head]->joint = Head;
  p.joints[Head]->upper_connected_joint = nullptr;
  p.joints[Head]->lower_connected_joint = p.joints[Neck];

  p.joints[Neck]->joint = Neck;
  p.joints[Neck]->upper_connected_joint = p.joints[Head];
  p.joints[Neck]->lower_connected_joint = p.joints[Shoulder];

  p.joints[Shoulder]->joint = Shoulder;
  p.joints[Shoulder]->upper_connected_joint = p.joints[Neck];
  p.joints[Shoulder]->lower_connected_joint = p.joints[Hip];

  p.joints[Hip]->joint = Hip;
  p.joints[Hip]->upper_connected_joint = p.joints[Shoulder];
  p.joints[Hip]->lower_connected_joint = p.joints[Knee];

  p.joints[Knee]->joint = Knee;
  p.joints[Knee]->upper_connected_joint = p.joints[Hip];
  p.joints[Knee]->lower_connected_joint = p.joints[Foot];

  p.joints[Foot]->joint = Foot;
  p.joints[Foot]->upper_connected_joint = p.joints[Knee];
  p.joints[Foot]->lower_connected_joint = nullptr;

  for (int i = 0; i <= JointMax; i++) {
    p.joints[i]->coord = {0, 0, PostProcessing::Untrustworthy};
    p.joints[i]->upper_angle = 0;
    p.joints[i]->lower_angle = 0;
  }

  return p;
}

void destroyPose(Pose p) {
  for (int i = 0; i < JointMax; i++) {
    free(p.joints[i]);
  }
}

PostureEstimator::PostureEstimator() {
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
  Pose p;
  return p;
}
void PostureEstimator::calculatePoseChanges() {
  for (int i = 0; i <= JointMax; i++) {
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

void PostureEstimator::calculateChangesAndCheckPosture() {
  calculatePoseChanges();
  checkGoodPosture();
}

}  // namespace PostureEstimating
