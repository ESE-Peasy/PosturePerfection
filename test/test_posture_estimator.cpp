#include <stdio.h>

#include <boost/test/unit_test.hpp>

// For testing private functions
#define private public
#include "../src/posture_estimator.h"
#undef private
#include <cmath>

PostureEstimating::Pose helper_create_pose() {
  PostureEstimating::Pose p = PostureEstimating::createPose();
  for (int i = JointMin; i <= JointMax; i++) {
    p.joints[i]->coord = PostProcessing::Coordinate{
        static_cast<float>(i), 1, PostProcessing::Trustworthy};
    p.joints[i]->lower_angle = -M_PI;
  }
  return p;
}
PostProcessing::ProcessedResults helper_create_result() {
  PostProcessing::ProcessedResults r;
  for (int i = JointMin; i <= JointMax; i++) {
    r.body_parts[i] =
        PostProcessing::Coordinate{static_cast<float>(i), static_cast<float>(i),
                                   PostProcessing::Trustworthy};
  }
  return r;
}

void helper_check_result(PostureEstimating::Pose p,
                         PostProcessing::ProcessedResults r) {
  for (int i = PostureEstimating::JointMin; i <= PostureEstimating::JointMax;
       i++) {
    BOOST_CHECK_EQUAL(p.joints[i]->joint, static_cast<Joint>(i));
    BOOST_CHECK_EQUAL(p.joints[i]->coord.status, r.body_parts[i].status);
    BOOST_CHECK_EQUAL(p.joints[i]->coord.y, r.body_parts[i].x);
    BOOST_CHECK_EQUAL(p.joints[i]->coord.y, r.body_parts[i].y);
    if (i > PostureEstimating::JointMin) {
      BOOST_CHECK_CLOSE(p.joints[i]->upper_angle, -3 * M_PI / 4, 0.00001);
    }
    if (i < PostureEstimating::JointMax) {
      BOOST_CHECK_CLOSE(p.joints[i]->lower_angle, M_PI / 4, 0.00001);
    }
  }
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
  e.current_pose.joints[PostureEstimating::JointMin]->lower_angle = -M_PI / 2;
  e.current_pose.joints[PostureEstimating::JointMin + 1]->upper_angle =
      M_PI / 2;
  e.calculatePoseChanges();

  BOOST_CHECK_EQUAL(
      e.pose_changes.joints[PostureEstimating::JointMin]->upper_angle, 0);
  BOOST_CHECK_CLOSE(
      e.pose_changes.joints[PostureEstimating::JointMin]->lower_angle,
      -M_PI / 2, 0.00001);
  BOOST_CHECK_CLOSE(
      e.pose_changes.joints[PostureEstimating::JointMin + 1]->upper_angle,
      -M_PI / 2, 0.00001);
  BOOST_CHECK_EQUAL(
      e.pose_changes.joints[PostureEstimating::JointMin + 1]->lower_angle, 0);

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
  for (int i = PostureEstimating::JointMin; i <= PostureEstimating::JointMax;
       i++) {
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

BOOST_AUTO_TEST_CASE(ChangesHandlesUntrustworthy) {
  PostureEstimating::PostureEstimator e;
  PostureEstimating::destroyPose(e.ideal_pose);
  PostureEstimating::destroyPose(e.current_pose);
  e.ideal_pose = helper_create_pose();
  e.current_pose = helper_create_pose();

  e.ideal_pose.joints[PostureEstimating::JointMin]->coord.status =
      PostProcessing::Untrustworthy;
  e.current_pose.joints[PostureEstimating::JointMin]->coord.status =
      PostProcessing::Untrustworthy;
  for (int i = PostureEstimating::JointMin + 1;
       i <= PostureEstimating::JointMin + 2; i++) {
    e.ideal_pose.joints[i]->coord.status = PostProcessing::Untrustworthy;
  }
  for (int i = PostureEstimating::JointMin + 3;
       i <= PostureEstimating::JointMin + 4; i++) {
    e.current_pose.joints[i]->coord.status = PostProcessing::Untrustworthy;
  }

  for (int i = PostureEstimating::JointMin; i <= PostureEstimating::JointMax;
       i++) {
    e.current_pose.joints[i]->upper_angle = 3 * M_PI / 2;
    e.current_pose.joints[i]->lower_angle = -3 * M_PI / 2;
  }

  e.calculatePoseChanges();
  for (int i = PostureEstimating::JointMin; i < PostureEstimating::JointMax;
       i++) {
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i]->upper_angle, 0);
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i]->lower_angle, 0);
  }
  BOOST_CHECK_CLOSE(
      e.pose_changes.joints[PostureEstimating::JointMax]->upper_angle,
      -3 * M_PI / 2, 0.00001);
  BOOST_CHECK_EQUAL(
      e.pose_changes.joints[PostureEstimating::JointMax]->lower_angle, 0);
}

BOOST_AUTO_TEST_CASE(GoodPostureNoChanges) {
  PostureEstimating::PostureEstimator e;
  e.pose_change_threshold = M_PI / 4;
  e.checkGoodPosture();

  BOOST_TEST(e.good_posture == true);
}

BOOST_AUTO_TEST_CASE(GoodPostureWithinThreshold) {
  PostureEstimating::PostureEstimator e;
  e.pose_changes.joints[PostureEstimating::JointMin]->lower_angle = M_PI / 6;
  e.pose_changes.joints[PostureEstimating::JointMin]->upper_angle = -M_PI / 6;
  e.pose_change_threshold = M_PI / 4;
  e.checkGoodPosture();

  BOOST_TEST(e.good_posture == true);
}

BOOST_AUTO_TEST_CASE(GoodPostureOnThreshold) {
  PostureEstimating::PostureEstimator e;
  e.pose_changes.joints[PostureEstimating::JointMin]->lower_angle = M_PI / 4;
  e.pose_changes.joints[PostureEstimating::JointMin + 1]->upper_angle =
      -M_PI / 4;
  e.pose_change_threshold = M_PI / 4;
  e.checkGoodPosture();

  BOOST_TEST(e.good_posture == true);
}
BOOST_AUTO_TEST_CASE(GoodPostureOutsideThreshold) {
  PostureEstimating::PostureEstimator e;
  e.pose_changes.joints[PostureEstimating::JointMin]->lower_angle = M_PI / 4;
  e.pose_changes.joints[PostureEstimating::JointMin + 1]->upper_angle =
      -M_PI / 4;
  e.pose_change_threshold = M_PI / 6;
  e.checkGoodPosture();

  BOOST_TEST(e.good_posture == false);
}

BOOST_AUTO_TEST_CASE(CreatePoseFromTrustworthyResults) {
  PostProcessing::ProcessedResults r = helper_create_result();
  PostureEstimating::PostureEstimator e;
  PostureEstimating::Pose p = e.createPoseFromResult(r);

  helper_check_result(p, r);
}

BOOST_AUTO_TEST_CASE(CreatePoseFromSomeTrustWorthyResults) {
  PostProcessing::ProcessedResults r = helper_create_result();
  PostureEstimating::PostureEstimator e;

  r.body_parts[PostureEstimating::Head].status = PostProcessing::Untrustworthy;
  r.body_parts[PostureEstimating::Knee].status = PostProcessing::Untrustworthy;
  r.body_parts[PostureEstimating::Foot].status = PostProcessing::Untrustworthy;
  PostureEstimating::Pose p = e.createPoseFromResult(r);
  helper_check_result(p, r);
}

BOOST_AUTO_TEST_CASE(CreatePoseFromNoTrustWorthyResults) {
  PostProcessing::ProcessedResults r = helper_create_result();
  PostureEstimating::PostureEstimator e;

  for (int i = PostureEstimating::JointMin; i <= PostureEstimating::JointMax;
       i++) {
    r.body_parts[i].status = PostProcessing::Untrustworthy;
  }

  PostureEstimating::Pose p = e.createPoseFromResult(r);

  helper_check_result(p, r);
}
