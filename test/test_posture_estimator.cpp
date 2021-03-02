#include <stdio.h>

#include <boost/test/unit_test.hpp>

// For testing private functions
#define private public
#include "../src/posture_estimator.h"
#undef private
#include <cmath>

PostureEstimating::Pose helper_create_pose() {
  PostureEstimating::Pose p = PostureEstimating::createPose();

  p.joints[PostureEstimating::Head]->coord =
      PostProcessing::Coordinate{1, 1, PostProcessing::Trustworthy};
  p.joints[PostureEstimating::Head]->lower_angle = -M_PI;
  p.joints[PostureEstimating::Head]->present = true;

  p.joints[PostureEstimating::Neck]->coord =
      PostProcessing::Coordinate{2, 1, PostProcessing::Trustworthy};
  p.joints[PostureEstimating::Neck]->lower_angle = -M_PI;
  p.joints[PostureEstimating::Neck]->present = true;

  p.joints[PostureEstimating::Shoulder]->coord =
      PostProcessing::Coordinate{3, 1, PostProcessing::Trustworthy};
  p.joints[PostureEstimating::Shoulder]->lower_angle = -M_PI;
  p.joints[PostureEstimating::Shoulder]->present = true;

  p.joints[PostureEstimating::Hip]->coord =
      PostProcessing::Coordinate{1, 1, PostProcessing::Trustworthy};
  p.joints[PostureEstimating::Hip]->lower_angle = -M_PI;
  p.joints[PostureEstimating::Hip]->present = true;

  p.joints[PostureEstimating::Knee]->coord =
      PostProcessing::Coordinate{1, 1, PostProcessing::Trustworthy};
  p.joints[PostureEstimating::Knee]->lower_angle = -M_PI;
  p.joints[PostureEstimating::Knee]->present = true;

  p.joints[PostureEstimating::Foot]->coord =
      PostProcessing::Coordinate{1, 1, PostProcessing::Trustworthy};
  p.joints[PostureEstimating::Foot]->present = true;

  return p;
}

BOOST_AUTO_TEST_CASE(LinesAngleCorrect) {
  PostProcessing::Coordinate p = {1, 1, PostProcessing::Trustworthy};
  PostProcessing::Coordinate p1 = {3, 4, PostProcessing::Trustworthy};
  PostureEstimating::PostureEstimator e;
  float angle = e.getLineAngle(p, p1);

  BOOST_CHECK_CLOSE(angle, (0.5880026035),
                    0.00001);  // Rounding due to using float
};

BOOST_AUTO_TEST_CASE(LinesAngleInAllQuads) {
  PostProcessing::Coordinate p = {0, 0, PostProcessing::Trustworthy};
  PostProcessing::Coordinate q1 = {2, 2, PostProcessing::Trustworthy};
  PostProcessing::Coordinate q2 = {-2, 2, PostProcessing::Trustworthy};
  PostProcessing::Coordinate q3 = {-2, -2, PostProcessing::Trustworthy};
  PostProcessing::Coordinate q4 = {2, -2, PostProcessing::Trustworthy};

  PostureEstimating::PostureEstimator e;
  float angleQ1 = e.getLineAngle(p, q1);
  float angleQ2 = e.getLineAngle(p, q2);
  float angleQ3 = e.getLineAngle(p, q3);
  float angleQ4 = e.getLineAngle(p, q4);

  BOOST_CHECK_CLOSE(angleQ1, ((45.0 / 180.0) * M_PI),
                    0.00001);  // Rounding due to using float
  BOOST_CHECK_CLOSE(angleQ4, ((135.0 / 180.0) * M_PI),
                    0.00001);  // Rounding due to using float
  BOOST_CHECK_CLOSE(angleQ2, (-45.0 / 180.0) * M_PI,
                    0.00001);  // Rounding due to using float
  BOOST_CHECK_CLOSE(angleQ3, ((-135.0 / 180.0) * M_PI),
                    0.00001);  // Rounding due to using float
};

BOOST_AUTO_TEST_CASE(LinesAngleSlopeZero) {
  PostProcessing::Coordinate p = {1, 1, PostProcessing::Trustworthy};
  PostProcessing::Coordinate h_p = {4, 1, PostProcessing::Trustworthy};
  PostProcessing::Coordinate v_p = {1, 4, PostProcessing::Trustworthy};
  PostProcessing::Coordinate h_n = {-4, 1, PostProcessing::Trustworthy};
  PostProcessing::Coordinate v_n = {1, -4, PostProcessing::Trustworthy};
  PostureEstimating::PostureEstimator e;

  BOOST_CHECK_CLOSE(e.getLineAngle(p, h_p), M_PI / 2,
                    0.00001);  // Rounding due to using float
  BOOST_CHECK_EQUAL(e.getLineAngle(p, v_p), 0);
  BOOST_CHECK_CLOSE(e.getLineAngle(p, h_n), -M_PI / 2,
                    0.00001);  // Rounding due to using float
  BOOST_CHECK_CLOSE(e.getLineAngle(p, v_n), -M_PI, 0.00001);
};

BOOST_AUTO_TEST_CASE(EmptyWorkingPoseChanges) {
  PostureEstimating::PostureEstimator e;
  e.calculatePoseChanges();

  for (int i = PostureEstimating::JointMin; i <= PostureEstimating::JointMax;
       i++) {
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i]->upper_angle, 0);
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i]->lower_angle, 0);
  }
}

BOOST_AUTO_TEST_CASE(NoDifferenceWorkingPoseChanges) {
  PostureEstimating::PostureEstimator e;
  PostureEstimating::destroyPose(e.ideal_pose);
  PostureEstimating::destroyPose(e.current_pose);
  e.ideal_pose = helper_create_pose();
  e.current_pose = helper_create_pose();
  e.calculatePoseChanges();

  for (int i = PostureEstimating::JointMin; i <= PostureEstimating::JointMax;
       i++) {
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i]->upper_angle, 0);
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i]->lower_angle, 0);
  }
}

BOOST_AUTO_TEST_CASE(WorkingPoseChanges) {
  PostureEstimating::PostureEstimator e;
  PostureEstimating::destroyPose(e.ideal_pose);
  PostureEstimating::destroyPose(e.current_pose);
  e.ideal_pose = helper_create_pose();
  e.current_pose = helper_create_pose();

  e.current_pose.joints[PostureEstimating::Head]->lower_angle = -M_PI / 2;
  e.current_pose.joints[PostureEstimating::Neck]->upper_angle = M_PI / 2;

  e.calculatePoseChanges();

  BOOST_CHECK_EQUAL(e.pose_changes.joints[PostureEstimating::Head]->upper_angle,
                    0);
  BOOST_CHECK_CLOSE(e.pose_changes.joints[PostureEstimating::Head]->lower_angle,
                    -M_PI / 2, 0.00001);
  BOOST_CHECK_CLOSE(e.pose_changes.joints[PostureEstimating::Neck]->upper_angle,
                    -M_PI / 2, 0.00001);
  BOOST_CHECK_EQUAL(e.pose_changes.joints[PostureEstimating::Neck]->lower_angle,
                    0);

  for (int i = PostureEstimating::Shoulder; i <= PostureEstimating::JointMax;
       i++) {
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i]->upper_angle, 0);
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i]->lower_angle, 0);
  }
}

BOOST_AUTO_TEST_CASE(ChangesHandlesNullPointers) {
  PostureEstimating::PostureEstimator e;
  PostureEstimating::destroyPose(e.ideal_pose);
  PostureEstimating::destroyPose(e.current_pose);
  e.ideal_pose = helper_create_pose();
  e.current_pose = helper_create_pose();

  for (int i = PostureEstimating::JointMin; i < PostureEstimating::Shoulder;
       i++) {
    e.ideal_pose.joints[i]->upper_connected_joint = nullptr;
    e.ideal_pose.joints[i]->lower_connected_joint = nullptr;
    e.current_pose.joints[i]->upper_connected_joint = nullptr;
    e.current_pose.joints[i]->lower_connected_joint = nullptr;
  }

  for (int i = PostureEstimating::Shoulder; i < PostureEstimating::Knee; i++) {
    e.ideal_pose.joints[i]->upper_connected_joint = nullptr;
    e.ideal_pose.joints[i]->lower_connected_joint = nullptr;
  }

  for (int i = PostureEstimating::Knee; i <= PostureEstimating::JointMax; i++) {
    e.current_pose.joints[i]->upper_connected_joint = nullptr;
    e.current_pose.joints[i]->lower_connected_joint = nullptr;
  }

  for (int i = 0; i <= PostureEstimating::JointMax; i++) {
    e.current_pose.joints[i]->upper_angle = 3 * M_PI / 2;
    e.current_pose.joints[i]->lower_angle = -3 * M_PI / 2;
  }

  e.calculatePoseChanges();

  for (int i = PostureEstimating::Shoulder; i <= PostureEstimating::JointMax;
       i++) {
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i]->upper_angle, 0);
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i]->lower_angle, 0);
  }
}

BOOST_AUTO_TEST_CASE(ChangesHandlesPresentFlag) {
  PostureEstimating::PostureEstimator e;
  PostureEstimating::destroyPose(e.ideal_pose);
  PostureEstimating::destroyPose(e.current_pose);
  e.ideal_pose = helper_create_pose();
  e.current_pose = helper_create_pose();

  for (int i = PostureEstimating::JointMin; i < PostureEstimating::Shoulder;
       i++) {
    e.ideal_pose.joints[i]->present = false;
    e.current_pose.joints[i]->present = false;
  }

  for (int i = PostureEstimating::Shoulder; i < PostureEstimating::Knee; i++) {
    e.ideal_pose.joints[i]->present = false;
  }

  for (int i = PostureEstimating::Knee; i <= PostureEstimating::JointMax; i++) {
    e.current_pose.joints[i]->present = false;
  }

  for (int i = 0; i <= PostureEstimating::JointMax; i++) {
    e.current_pose.joints[i]->upper_angle = 3 * M_PI / 2;
    e.current_pose.joints[i]->lower_angle = -3 * M_PI / 2;
  }

  e.calculatePoseChanges();

  for (int i = PostureEstimating::Shoulder; i <= PostureEstimating::JointMax;
       i++) {
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i]->upper_angle, 0);
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i]->lower_angle, 0);
  }
}

BOOST_AUTO_TEST_CASE(GoodPostureNoChanges) {
  PostureEstimating::PostureEstimator e;
  PostureEstimating::destroyPose(e.ideal_pose);
  PostureEstimating::destroyPose(e.current_pose);
  e.ideal_pose = helper_create_pose();
  e.current_pose = helper_create_pose();
  e.calculatePoseChanges();

  e.checkGoodPosture();
  BOOST_TEST(e.good_posture == true);
}

BOOST_AUTO_TEST_CASE(GoodPostureWithinThreshold) {
  PostureEstimating::PostureEstimator e;
  PostureEstimating::destroyPose(e.ideal_pose);
  PostureEstimating::destroyPose(e.current_pose);
  e.ideal_pose = helper_create_pose();
  e.current_pose = helper_create_pose();
  e.calculatePoseChanges();

  e.current_pose.joints[PostureEstimating::Head]->lower_angle = -5 * M_PI / 6;
  e.current_pose.joints[PostureEstimating::Neck]->upper_angle = -5 * M_PI / 6;

  e.pose_change_threshold = M_PI / 4;
  e.checkGoodPosture();
  BOOST_TEST(e.good_posture == true);
}

BOOST_AUTO_TEST_CASE(GoodPostureOnThreshold) {
  PostureEstimating::PostureEstimator e;
  PostureEstimating::destroyPose(e.ideal_pose);
  PostureEstimating::destroyPose(e.current_pose);
  e.ideal_pose = helper_create_pose();
  e.current_pose = helper_create_pose();
  e.calculatePoseChanges();

  e.current_pose.joints[PostureEstimating::Head]->lower_angle = -5 * M_PI / 6;
  e.current_pose.joints[PostureEstimating::Neck]->upper_angle = -5 * M_PI / 6;

  e.pose_change_threshold = M_PI / 6;
  e.checkGoodPosture();
  BOOST_TEST(e.good_posture == true);
}

BOOST_AUTO_TEST_CASE(GoodPostureOutsideThreshold) {
  PostureEstimating::PostureEstimator e;
  PostureEstimating::destroyPose(e.ideal_pose);
  PostureEstimating::destroyPose(e.current_pose);
  e.ideal_pose = helper_create_pose();
  e.current_pose = helper_create_pose();
  e.calculatePoseChanges();

  e.current_pose.joints[PostureEstimating::Head]->lower_angle = -M_PI / 2;
  e.current_pose.joints[PostureEstimating::Neck]->upper_angle = M_PI / 2;

  e.pose_change_threshold = M_PI / 6;
  e.checkGoodPosture();
  BOOST_TEST(e.good_posture == true);
}