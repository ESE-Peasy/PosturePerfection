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

#include <CppTimer.h>
#include <stdio.h>

#include <string>
#include <vector>

#include "intermediate_structures.h"
#include "notifications/broadcaster.h"
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
 * @brief Potential states which the posture can be. `Unset` means that the
 * `ideal_pose` has not been set by the user.
 *
 */
enum PostureState { Good, Bad, Unset, Undefined, UndefinedAndUnset };

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
  PostureState posture_state;
};

/**
 * @brief Colours corresponding to `PostureEstimator::colours`
 */
enum Colours { Red, Green, Blue, Grey };

/**
 * @brief Simple timer which just has the running flag set when running and
 * unset when not running
 */
class DelayTimer : public CppTimer {
 private:
 public:
  size_t time;  ///< The time timer runs for
  bool running =
      false;  ///< Boolen which indicates if timer is running (True = running)
  /**
   * @brief Constructor for `PostureEstimating::DelayTimer`
   * @param time The time timer will run for
   */
  explicit DelayTimer(size_t time);
  /**
   * Deconstructor for `PostureEstimating::DelayTimer`
   */
  ~DelayTimer();
  /**
   * @brief Sets running flag to false
   */
  void timerEvent();
  /**
   * @brief Starts the delay timer running
   */
  void countdown();
};
/**
 * @brief Broadcasts a message using `Notify::NotifyBroadcast` after a time is
 * elapsed
 */
class MessageTimer : public CppTimer {
 private:
  std::vector<DelayTimer*>
      notificationTimers;  ///< List of `PostureEstimating::DelayTimers` that
                           ///< must not be running for message to be broadcast
  Notify::NotifyBroadcast*
      broadcaster;  ///< `Notify::NotifyBroadcast` used to broadcast message
  std::string msg;  ///< Message to be sent

 public:
  size_t time;  ///< The time the `PostureEstimating::MessageTimer` waits
                ///< before broadcasting message
  bool running =
      false;  ///< Boolen which indicates if timer is running (True = running)
  /**
   * @brief Constructor for `PostureEstimating::MessageTimer`
   * @param notificationTimers List of `PostureEstimating::DelayTimers` that
   * must not be running for message to be broadcast
   * @param broadcaster  `Notify::NotifyBroadcast` used to broadcast the message
   * @param msg message to be sent
   * @param time The time to wait before broadcasting message
   */
  MessageTimer(std::vector<DelayTimer*> timers,
               Notify::NotifyBroadcast* broadcast, std::string msg,
               size_t time);
  /**
   * @brief Deconstructor for `PostureEstimating::MessageTimer`
   */
  ~MessageTimer();
  /**
   * @brief Starts timer to countdown, sending a messsage at the end of the
   * countdown
   */
  void countdown();
  /**
   * @brief Stops the countdown and doesn't send message
   */
  void stopCountdown();
  /**
   * Broadcasts message using `Notify::NotifyBroadcast`
   */
  void timerEvent();
};

/**
 * @brief Timer which countdowns and stops a `PostureEstimating::MessageTimer`
 * if countdown ends
 */
class StopTimer : public CppTimer {
 private:
  MessageTimer* toStop;  ///< MessageTimer countdown to stop

 public:
  size_t time;  ///< The time to wait before stopping MessageTimer countdown
  bool running =
      false;  ///< Boolen which indicates if timer is running (True = running)
  /**
   * @brief Constructor for `PostureEstimating::StopTimer`
   * @param toStop `PostureEstimating::MessageTimer` that is stopped when
   * countdown finishes
   * @param time The time countdown runs for before stopping
   * `PostureEstimating::MessageTimer` countdown
   */
  StopTimer(MessageTimer* toStop, size_t time);
  /**
   * @brief Deconstructor for `PostureEstimating::StopTimer`
   */
  ~StopTimer();
  /**
   * @brief Starts countdown, stopping `PostureEstimating::MessageTimer` if
   * countdown reached
   */
  void countdown();
  /**
   * @brief Stops the countdown and doesn't stop
   * `PostureEstimating::MessageTimer`
   */
  void stopCountdown();
  /**
   * @brief Runs stopCountdown() on `PostureEstimating::MessageTimer`
   */
  void timerEvent();
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
 * General use should just consist of `runEstimator`.
 *
 */
class PostureEstimator {
 private:
  /**
   * @brief Array of colours used when indicating posture
   *
   */
  std::array<cv::Scalar, 4> colours = {
      cv::Scalar(255, 0, 0), cv::Scalar(0, 255, 0), cv::Scalar(0, 0, 255),
      cv::Scalar(144, 144, 144)};

  /**
   * @brief NotifySend broadcaster for sending messages
   */
  Notify::NotifyBroadcast broadcaster;
  DelayTimer badPostureNotificationTimer;
  DelayTimer undefinedPostureNotificationTimer;
  MessageTimer badPostureTimer;
  MessageTimer undefinedPostureTimer;
  StopTimer stopBadPostureTimer;
  StopTimer stopUndefinedPostureTimer;

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
   * @brief Determine the current `PostureEstimating::PostureState` based on
   * the `current_pose` and the `ideal_pose`. This works as follows:
   * - If `posture_state` is currently `Unset` then remain in this state until
   * the user sets their `ideal_pose`
   * - If there are no consecutive nodes that are `Trustworthy` then the state
   * is changed to `Undefined` or `UndefinedAndUnset` depending on if an
   * `ideal_pose` has been set or not
   * - If the current state is not `Undefined` and the `pose_changes` angles
   * are outwith the `pose_change_threshold` then the state is changed to
   * `Bad`
   * - If none of these conditions are met, then the state is changed to
   * `Good`.
   *
   * Note that this means it is possible for only partial segments of the
   * overall posture to be displayed
   *
   */
  void checkPostureState();

  /**
   * @brief Calculates current posture and ideal posture difference and
   * decides if a good posture has been adopted based on threshold values.
   *
   */
  void calculateChangesAndCheckPosture();

  /**
   * @brief Updates the user's current pose from
   * `PostProcessing::ProcessingResults`, Calculates pose change needed.
   * Checks if pose change needed is outside threshold.
   *
   * @param results `PostProcessing::ProcessingResults` struct containing
   * user's pose data.
   * @return `PostureEstimating::PostureState`
   */
  PostureEstimating::PostureState updateCurrentPoseAndCheckPosture(
      PostProcessing::ProcessedResults results);

  /**
   * @brief Overlay the current pose for the current frame
   *
   * @param current_pose `PostureEstimating::Pose` The pose for the current
   * posture
   * @param current_frame `cv::Mat` Current frame to overlay lines onto
   * @param posture_state `PostureEstimating::PostureState` If `Good` draw
   * green lines to indicate good posture, and if `Unset` draw the
   * `ideal_pose` has not been set in which case draw blue lines
   */
  void display_current_pose(PostureEstimating::Pose current_pose,
                            cv::Mat current_frame,
                            PostureEstimating::PostureState posture_state);
  /**
   * @brief Overlay the pose changes for the current frame. We only use this
   * if the `posture_state` in `pose_status` is `Bad`.
   *
   * @param pose_changes `PostureEstimating::Pose` Changes needed to return to
   * a good posture
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
  ~PostureEstimator();

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
   * @brief Whether the user is currently in a `Good`, `Bad`, `Unset`,
   * `Undefined` or `UndefinedAndUnset` posture.
   * (`Unset` means that the `ideal_pose` has not yet been set, and
   * `Undefined` is if pose estimation has not found any successive
   * co-ordinates which are `Trustworthy`)
   */
  PostureEstimating::PostureState posture_state = UndefinedAndUnset;

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
   * @param results `PostProcessing::ProcessedResults` struct containing
   * user's pose data.
   */
  PoseStatus runEstimator(PostProcessing::ProcessedResults results);

  /**
   * @brief Analyse the `posture_state` field of
   * `PostureEstimating::PoseStatus` and use it as follows:
   *  - If `Unset` then indicate this to user by
   * displaying the current pose as blue lines.
   * - If `Good` then display current pose as green lines.
   * - If `Bad` then use `pose_change_threshold` and
   * `pose_changes` to indicate which direction the user needs to move in
   * order to return to a good posture.
   *
   * @param pose_status `PostureEstimating::PoseStatus` The pose status for
   * the current frame
   * @param current_frame `cv::Mat` The current frame to overlay lines on to
   */
  void analysePosture(PostureEstimating::PoseStatus pose_status,
                      cv::Mat current_frame);
};
}  // namespace PostureEstimating
#endif  // SRC_POSTURE_ESTIMATOR_H_
