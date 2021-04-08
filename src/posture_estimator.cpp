/**
 * @copyright Copyright (C) 2021  Conor Begley, Ashwin Maliampurakal
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

#define BAD_POSTURE_TIME 10000000000        ///< 10 seconds
#define UNDEFINED_POSTURE_TIME 15000000000  ///< 15 seconds
#define GOOD_POSTURE_TIME 5000000000        ///< 5 seconds
#define NOTIFICATION_TIME 1200000000000     ///< 120 seconds

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

PostureEstimator::PostureEstimator()
    : broadcaster(),
      notificationTimer(NOTIFICATION_TIME),
      badPostureTimer(
          &notificationTimer, &broadcaster,
          "You have an imperfect posture, consider readjusting to achieve "
          "posture perfection",
          BAD_POSTURE_TIME),
      undefinedPostureTimer(&notificationTimer, &broadcaster,
                            "Are you still there?", UNDEFINED_POSTURE_TIME),
      cancelBadPostureTimer(&badPostureTimer, GOOD_POSTURE_TIME),
      cancelUndefinedPostureTimer(&badPostureTimer, GOOD_POSTURE_TIME) {
  this->pose_change_threshold = 0.1;
  this->ideal_pose = createPose();
  this->current_pose = createPose();
  this->pose_changes = createPose();
  this->broadcaster.sendMessage("Posture Perfection is now running");
}

PostureEstimator::~PostureEstimator() {
  this->broadcaster.sendMessage("Posture Perfection has shutdown");
}

float PostureEstimator::getLineAngle(PostProcessing::Coordinate coord1,
                                     PostProcessing::Coordinate coord2) {
  float x_dif = (coord2.x - 0.5) - (coord1.x - 0.5);
  float y_dif = ((1.0 - coord2.y) - 0.5) - ((1.0 - coord1.y) - 0.5);

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
  if (this->posture_state == Unset) {
    // If the ideal posture has not been set, then remain in the Unset
    // state regardless
    return;
  }

  bool trustworthyPosture = true;
  for (int i = JointMin; i <= JointMax - 2; i++) {
    // If any detected joint is Untrustworthy, then the overall
    // posture is untrustworthy
    if (this->current_pose.joints.at(i).coord.status ==
        PostProcessing::Untrustworthy) {
      trustworthyPosture = false;
    }
  }

  if (!trustworthyPosture) {
    this->posture_state = Undefined;
    return;
  }

  // If the overall posture is trustworthy then check if it is a Bad
  // or Good posture
  for (int i = JointMin + 1; i <= JointMax - 2; i++) {
    if ((fabs(this->pose_changes.joints[i].upper_angle) <
         -(this->pose_change_threshold)) ||
        fabs(this->pose_changes.joints[i].upper_angle) >
            this->pose_change_threshold) {
      this->posture_state = Bad;
      return;
    }
  }
  this->posture_state = Good;
}

void PostureEstimator::calculateChangesAndCheckPosture() {
  calculatePoseChanges();
  checkGoodPosture();
}

PostureEstimating::PostureState
PostureEstimator::updateCurrentPoseAndCheckPosture(
    PostProcessing::ProcessedResults results) {
  update_current_pose(results);
  calculateChangesAndCheckPosture();
  return this->posture_state;
}

void PostureEstimator::display_current_pose(
    PostureEstimating::Pose current_pose, cv::Mat current_frame,
    PostureEstimating::PostureState posture_state) {
  int imageWidth = current_frame.cols;
  int imageHeight = current_frame.rows;

  for (int i = JointMin + 1; i <= JointMax - 2; i++) {
    // Only consider the Head, Neck, Shoulder and Hip joints
    if (current_pose.joints.at(i).coord.status == PostProcessing::Trustworthy &&
        current_pose.joints.at(i - 1).coord.status ==
            PostProcessing::Trustworthy) {
      cv::Point upper_joint_point(
          static_cast<int>(current_pose.joints.at(i - 1).coord.x * imageWidth),
          static_cast<int>(current_pose.joints.at(i - 1).coord.y *
                           imageHeight));

      cv::Point current_joint_point(
          static_cast<int>(current_pose.joints.at(i).coord.x * imageWidth),
          static_cast<int>(current_pose.joints.at(i).coord.y * imageHeight));

      cv::line(current_frame, upper_joint_point, current_joint_point,
               posture_state == Good ? colours.at(Green) : colours.at(Blue), 5);
    }
  }
}

void PostureEstimator::display_pose_changes_needed(
    PostureEstimating::Pose pose_changes, PostureEstimating::Pose current_pose,
    cv::Mat current_frame) {
  int imageWidth = current_frame.cols;
  int imageHeight = current_frame.rows;
  for (int i = JointMin + 1; i <= JointMax - 2; i++) {
    // Only consider the Head, Neck, Shoulder and Hip joints
    if (current_pose.joints.at(i).coord.status == PostProcessing::Trustworthy &&
        current_pose.joints.at(i - 1).coord.status ==
            PostProcessing::Trustworthy) {
      cv::Point upper_joint_point(
          static_cast<int>(current_pose.joints.at(i - 1).coord.x * imageWidth),
          static_cast<int>(current_pose.joints.at(i - 1).coord.y *
                           imageHeight));

      cv::Point current_joint_point(
          static_cast<int>(current_pose.joints.at(i).coord.x * imageWidth),
          static_cast<int>(current_pose.joints.at(i).coord.y * imageHeight));

      cv::Point midpoint(
          static_cast<int>((upper_joint_point.x + current_joint_point.x) / 2),
          static_cast<int>((upper_joint_point.y + current_joint_point.y) / 2));

      // Indicate directions to fix posture for each joint
      if (pose_changes.joints.at(i).upper_angle > pose_change_threshold) {
        cv::line(current_frame, upper_joint_point, current_joint_point,
                 colours.at(Red), 5);
        cv::Point tip(static_cast<int>(midpoint.x + 50),
                      static_cast<int>(midpoint.y));

        cv::arrowedLine(current_frame, midpoint, tip, colours.at(Blue), 2, 8, 0,
                        0.2);
      } else if (pose_changes.joints.at(i).upper_angle <
                 -pose_change_threshold) {
        cv::line(current_frame, upper_joint_point, current_joint_point,
                 colours.at(Red), 5);
        cv::Point tip(static_cast<int>(midpoint.x - 50),
                      static_cast<int>(midpoint.y));

        cv::arrowedLine(current_frame, midpoint, tip, colours.at(Blue), 2, 8, 0,
                        0.2);
      } else {
        cv::line(current_frame, upper_joint_point, current_joint_point,
                 colours.at(Green), 5);
      }
    }
  }
}

void PostureEstimator::update_ideal_pose(PostureEstimating::Pose pose) {
  this->posture_state = Good;
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

float PostureEstimator::get_pose_change_threshold(void) {
  return this->pose_change_threshold;
}

PoseStatus PostureEstimator::runEstimator(
    PostProcessing::ProcessedResults results) {
  this->updateCurrentPoseAndCheckPosture(results);
  PoseStatus p = {this->ideal_pose, this->current_pose, this->pose_changes,
                  this->posture_state};
  return p;
}

void PostureEstimator::analysePosture(PostureEstimating::PoseStatus pose_status,
                                      cv::Mat current_frame) {
  PostureEstimating::Pose current_pose = pose_status.current_pose;
  PostureEstimating::Pose pose_changes = pose_status.pose_changes;
  PostureEstimating::PostureState posture_state = pose_status.posture_state;

  cv::cvtColor(current_frame, current_frame, cv::COLOR_BGR2RGB);

  if (posture_state == Undefined) {
    if (!this->undefinedPostureTimer.running) {
      this->undefinedPostureTimer.countdown();
    }
    if (this->cancelUndefinedPostureTimer.running) {
      this->cancelUndefinedPostureTimer.stopCountdown();
    }
  }
  if (posture_state == Bad) {
    if (!this->badPostureTimer.running) {
      this->badPostureTimer.countdown();
    }
    if (this->cancelBadPostureTimer.running) {
      this->cancelBadPostureTimer.stopCountdown();
    }
    display_pose_changes_needed(pose_changes, current_pose, current_frame);
  } else {
    if (posture_state == Good) {
      if (this->badPostureTimer.running) {
        this->cancelBadPostureTimer.countdown();
      }
      if (this->undefinedPostureTimer.running) {
        this->cancelUndefinedPostureTimer.countdown();
      }
    }
    display_current_pose(current_pose, current_frame, posture_state);
  }
}

DelayTimer::DelayTimer(size_t time) : CppTimer() { this->time = time; }
DelayTimer::~DelayTimer() {}
void DelayTimer::timerEvent() { this->running = false; }
void DelayTimer::countdown() {
  this->running = true;
  this->start(this->time, ONESHOT);
}
cancelTimer::cancelTimer(MessageTimer* toCancel, size_t time) : CppTimer() {
  this->toCancel = toCancel;
  this->time = time;
}
cancelTimer::~cancelTimer() {}
void cancelTimer::countdown() {
  if (!this->running) {
    this->running = true;
    this->start(this->time, ONESHOT);
  }
}
void cancelTimer::stopCountdown() {
  if (this->running) {
    this->running = false;
    this->stop();
  }
}
void cancelTimer::timerEvent() {
  this->running = false;
  this->toCancel->stopCountdown();
}

MessageTimer::MessageTimer(DelayTimer* timer,
                           Notify::NotifyBroadcast* broadcast, std::string msg,
                           size_t time)
    : CppTimer() {
  this->notificationTimer = timer;
  this->broadcaster = broadcast;
  this->msg = msg;
  this->time = time;
}
MessageTimer::~MessageTimer() {}
void MessageTimer::countdown() {
  if (!this->running) {
    this->running = true;
    this->start(this->time, ONESHOT);
  }
}
void MessageTimer::stopCountdown() {
  if (this->running) {
    this->running = false;
    this->stop();
  }
}
void MessageTimer::timerEvent() {
  if (!this->notificationTimer->running) {
    this->notificationTimer->start(NOTIFICATION_TIME, ONESHOT);
    this->broadcaster->sendMessage(this->msg);
  }
}
};  // namespace PostureEstimating
