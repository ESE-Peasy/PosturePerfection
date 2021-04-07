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
    p.joints[i].coord = PostProcessing::Coordinate{
        static_cast<float>(i) / JointMax, 0.0, PostProcessing::Trustworthy};
    p.joints[i].lower_angle = -M_PI;
  }
  return p;
}
PostProcessing::ProcessedResults helper_create_result() {
  PostProcessing::ProcessedResults r;
  for (int i = JointMin; i <= JointMax; i++) {
    r.body_parts[i] = PostProcessing::Coordinate{
        static_cast<float>(i * 1) / (JointMax * 2),
        static_cast<float>(1.0 - (static_cast<float>(i * -1) / (JointMax * 2))),
        PostProcessing::Trustworthy};
  }
  return r;
}

void helper_check_result(PostureEstimating::Pose p,
                         PostProcessing::ProcessedResults r) {
  for (int i = JointMin; i <= JointMax; i++) {
    BOOST_CHECK_EQUAL(p.joints[i].joint, static_cast<Joint>(i));
    BOOST_CHECK_EQUAL(p.joints[i].coord.status, r.body_parts[i].status);
    BOOST_CHECK_EQUAL(p.joints[i].coord.x, r.body_parts[i].x);
    BOOST_CHECK_EQUAL(p.joints[i].coord.y, r.body_parts[i].y);
    if (i > JointMin) {
      BOOST_CHECK_CLOSE(p.joints[i].upper_angle, -M_PI / 4, 0.0001);
    }
    if (i < JointMax) {
      BOOST_CHECK_CLOSE(p.joints[i].lower_angle, 3 * M_PI / 4, 0.0001);
    }
  }
}

BOOST_AUTO_TEST_CASE(LinesAngleCorrect) {
  PostProcessing::Coordinate p = {0.25, 0.75, PostProcessing::Trustworthy};
  PostProcessing::Coordinate p1 = {0.75, 0.0, PostProcessing::Trustworthy};
  PostureEstimating::PostureEstimator e;
  float angle = e.getLineAngle(p, p1);

  BOOST_CHECK_CLOSE(angle, (0.5880026035),
                    0.00001);  // Rounding due to using float
}

BOOST_AUTO_TEST_CASE(LinesAngleInAllQuads) {
  PostProcessing::Coordinate p = {0.5, 0.5, PostProcessing::Trustworthy};
  PostProcessing::Coordinate q1 = {1.0, 0.0, PostProcessing::Trustworthy};
  PostProcessing::Coordinate q2 = {0.0, 0.0, PostProcessing::Trustworthy};
  PostProcessing::Coordinate q3 = {0.0, 1.0, PostProcessing::Trustworthy};
  PostProcessing::Coordinate q4 = {1.0, 1.0, PostProcessing::Trustworthy};
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
}

BOOST_AUTO_TEST_CASE(LinesAngleSlopeZero) {
  PostProcessing::Coordinate p = {5.0 / 8.0, 3.0 / 8.0,
                                  PostProcessing::Trustworthy};
  PostProcessing::Coordinate h_p = {1.0, 3.0 / 8.0,
                                    PostProcessing::Trustworthy};
  PostProcessing::Coordinate v_p = {5.0 / 8.0, 0.0,
                                    PostProcessing::Trustworthy};
  PostProcessing::Coordinate h_n = {0.0, 3.0 / 8.0,
                                    PostProcessing::Trustworthy};
  PostProcessing::Coordinate v_n = {5.0 / 8.0, 1.0,
                                    PostProcessing::Trustworthy};
  PostureEstimating::PostureEstimator e;

  BOOST_CHECK_CLOSE(e.getLineAngle(p, h_p), M_PI / 2,
                    0.00001);  // Rounding due to using float
  BOOST_CHECK_EQUAL(e.getLineAngle(p, v_p), 0);
  BOOST_CHECK_CLOSE(e.getLineAngle(p, h_n), -M_PI / 2,
                    0.00001);  // Rounding due to using float
  BOOST_CHECK_CLOSE(e.getLineAngle(p, v_n), -M_PI, 0.00001);
}

BOOST_AUTO_TEST_CASE(EmptyWorkingPoseChanges) {
  PostureEstimating::PostureEstimator e;
  e.calculatePoseChanges();

  for (int i = JointMin; i <= JointMax; i++) {
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i].upper_angle, 0);
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i].lower_angle, 0);
  }
}

BOOST_AUTO_TEST_CASE(NoDifferenceWorkingPoseChanges) {
  PostureEstimating::PostureEstimator e;
  e.ideal_pose = helper_create_pose();
  e.current_pose = helper_create_pose();
  e.calculatePoseChanges();

  for (int i = JointMin; i <= JointMax; i++) {
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i].upper_angle, 0);
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i].lower_angle, 0);
  }
}

BOOST_AUTO_TEST_CASE(WorkingPoseChanges) {
  PostureEstimating::PostureEstimator e;
  e.ideal_pose = helper_create_pose();
  e.current_pose = helper_create_pose();
  e.current_pose.joints[JointMin].lower_angle = -M_PI / 2;
  e.current_pose.joints[JointMin + 1].upper_angle = M_PI / 2;
  e.calculatePoseChanges();

  BOOST_CHECK_EQUAL(e.pose_changes.joints[JointMin].upper_angle, 0);
  BOOST_CHECK_CLOSE(e.pose_changes.joints[JointMin].lower_angle, -M_PI / 2,
                    0.00001);
  BOOST_CHECK_CLOSE(e.pose_changes.joints[JointMin + 1].upper_angle, -M_PI / 2,
                    0.00001);
  BOOST_CHECK_EQUAL(e.pose_changes.joints[JointMin + 1].lower_angle, 0);

  for (int i = JointMin + 2; i <= JointMax; i++) {
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i].upper_angle, 0);
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i].lower_angle, 0);
  }
}

BOOST_AUTO_TEST_CASE(ChangesHandlesUnTrustWorthy) {
  PostureEstimating::PostureEstimator e;
  e.ideal_pose = helper_create_pose();
  e.current_pose = helper_create_pose();

  e.ideal_pose.joints[JointMin].coord.status = PostProcessing::Untrustworthy;
  e.current_pose.joints[JointMin].coord.status = PostProcessing::Untrustworthy;
  e.ideal_pose.joints[JointMin + 2].coord.status =
      PostProcessing::Untrustworthy;
  e.current_pose.joints[JointMin + 4].coord.status =
      PostProcessing::Untrustworthy;

  for (int i = JointMin; i <= JointMax; i++) {
    e.current_pose.joints[i].upper_angle = 3 * M_PI / 2;
    e.current_pose.joints[i].lower_angle = -3 * M_PI / 2;
  }

  e.calculatePoseChanges();
  for (int i = JointMin; i <= JointMax; i++) {
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i].upper_angle, 0);
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i].lower_angle, 0);
  }
}

BOOST_AUTO_TEST_CASE(ChangesHandlesUntrustworthy) {
  PostureEstimating::PostureEstimator e;
  e.ideal_pose = helper_create_pose();
  e.current_pose = helper_create_pose();

  e.ideal_pose.joints[JointMin].coord.status = PostProcessing::Untrustworthy;
  e.current_pose.joints[JointMin].coord.status = PostProcessing::Untrustworthy;
  for (int i = JointMin + 1; i <= JointMin + 2; i++) {
    e.ideal_pose.joints[i].coord.status = PostProcessing::Untrustworthy;
  }
  for (int i = JointMin + 3; i <= JointMin + 4; i++) {
    e.current_pose.joints[i].coord.status = PostProcessing::Untrustworthy;
  }

  for (int i = JointMin; i <= JointMax; i++) {
    e.current_pose.joints[i].upper_angle = 3 * M_PI / 2;
    e.current_pose.joints[i].lower_angle = -3 * M_PI / 2;
  }

  e.calculatePoseChanges();
  for (int i = JointMin; i <= JointMax; i++) {
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i].upper_angle, 0);
    BOOST_CHECK_EQUAL(e.pose_changes.joints[i].lower_angle, 0);
  }
}

BOOST_AUTO_TEST_CASE(StartInUnsetPostureState) {
  PostureEstimating::PostureEstimator e;

  BOOST_TEST(e.posture_state == PostureEstimating::Unset);
}

BOOST_AUTO_TEST_CASE(AllUntrustworthyJointsGivesUndefinedPostureState) {
  PostureEstimating::PostureEstimator e;
  // current_pose is initialised as a Pose with all Untrustworthy joints
  e.update_ideal_pose(e.current_pose);
  e.checkPostureState();

  BOOST_TEST(e.posture_state == PostureEstimating::Undefined);
}

BOOST_AUTO_TEST_CASE(SettingIdealPostureChangesState) {
  PostureEstimating::PostureEstimator e;
  e.checkPostureState();

  // Start in Unset state
  BOOST_TEST(e.posture_state == PostureEstimating::Unset);

  e.current_pose = helper_create_pose();
  e.update_ideal_pose(e.current_pose);
  e.checkPostureState();

  BOOST_TEST(e.posture_state == PostureEstimating::Good);
}

BOOST_AUTO_TEST_CASE(GoodPostureNoChanges) {
  PostureEstimating::PostureEstimator e;
  e.current_pose = helper_create_pose();
  e.update_ideal_pose(e.current_pose);
  e.pose_change_threshold = M_PI / 4;
  e.checkPostureState();

  BOOST_TEST(e.posture_state == PostureEstimating::Good);
}

BOOST_AUTO_TEST_CASE(GoodPostureWithinThreshold) {
  PostureEstimating::PostureEstimator e;
  e.current_pose = helper_create_pose();
  e.update_ideal_pose(e.current_pose);
  e.pose_changes.joints[JointMin].lower_angle = M_PI / 6;
  e.pose_changes.joints[JointMin].upper_angle = -M_PI / 6;
  e.pose_change_threshold = M_PI / 4;
  e.checkPostureState();

  BOOST_TEST(e.posture_state == PostureEstimating::Good);
}

BOOST_AUTO_TEST_CASE(GoodPostureOnThreshold) {
  PostureEstimating::PostureEstimator e;
  e.current_pose = helper_create_pose();
  e.update_ideal_pose(e.current_pose);
  e.pose_changes.joints[JointMin].lower_angle = M_PI / 4;
  e.pose_changes.joints[JointMin + 1].upper_angle = -M_PI / 4;
  e.pose_change_threshold = M_PI / 4;
  e.checkPostureState();

  BOOST_TEST(e.posture_state == PostureEstimating::Good);
}
BOOST_AUTO_TEST_CASE(GoodPostureOutsideThreshold) {
  PostureEstimating::PostureEstimator e;
  e.current_pose = helper_create_pose();
  e.update_ideal_pose(e.current_pose);
  e.pose_changes.joints[JointMin].lower_angle = M_PI / 4;
  e.pose_changes.joints[JointMin + 1].upper_angle = -M_PI / 4;
  e.pose_change_threshold = M_PI / 6;
  e.checkPostureState();

  BOOST_TEST(e.posture_state == PostureEstimating::Bad);
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

  r.body_parts[Head].status = PostProcessing::Untrustworthy;
  r.body_parts[Knee].status = PostProcessing::Untrustworthy;
  r.body_parts[Foot].status = PostProcessing::Untrustworthy;
  PostureEstimating::Pose p = e.createPoseFromResult(r);
  helper_check_result(p, r);
}

BOOST_AUTO_TEST_CASE(CreatePoseFromNoTrustWorthyResults) {
  PostProcessing::ProcessedResults r = helper_create_result();
  PostureEstimating::PostureEstimator e;

  for (int i = JointMin; i <= JointMax; i++) {
    r.body_parts[i].status = PostProcessing::Untrustworthy;
  }

  PostureEstimating::Pose p = e.createPoseFromResult(r);

  helper_check_result(p, r);
}

BOOST_AUTO_TEST_CASE(CreateProducedPoseStatusStructure) {
  PostProcessing::ProcessedResults ri = helper_create_result();
  PostProcessing::ProcessedResults rc = helper_create_result();
  PostureEstimating::PostureEstimator e;
  e.update_ideal_pose(e.createPoseFromResult(ri));

  PostureEstimating::PoseStatus p = e.runEstimator(rc);

  helper_check_result(p.ideal_pose, ri);
  helper_check_result(p.current_pose, rc);

  BOOST_CHECK_EQUAL(p.posture_state, PostureEstimating::Good);
  PostProcessing::ProcessedResults rch = helper_create_result();
  rch.body_parts[JointMin + 2] = {5, 3, PostProcessing::Trustworthy};
  PostureEstimating::PoseStatus pc = e.runEstimator(rch);
  BOOST_CHECK_EQUAL(pc.posture_state, PostureEstimating::Bad);

  for (int i = JointMin; i <= JointMax; i++) {
    BOOST_CHECK_EQUAL(pc.pose_changes.joints[i].joint, static_cast<Joint>(i));
    if (JointMin < i && i < JointMin + 4) {
      continue;
    }
    BOOST_CHECK_EQUAL(pc.pose_changes.joints[i].upper_angle, 0);
    BOOST_CHECK_EQUAL(pc.pose_changes.joints[i].lower_angle, 0);
  }
  BOOST_CHECK_EQUAL(pc.pose_changes.joints[JointMin + 1].upper_angle, 0);
  BOOST_CHECK_NE(pc.pose_changes.joints[JointMin + 1].lower_angle, 0);
  BOOST_CHECK_NE(pc.pose_changes.joints[JointMin + 3].upper_angle, 0);
  BOOST_CHECK_EQUAL(pc.pose_changes.joints[JointMin + 3].lower_angle, 0);
  BOOST_CHECK_NE(pc.pose_changes.joints[JointMin + 2].upper_angle, 0);
  BOOST_CHECK_NE(pc.pose_changes.joints[JointMin + 2].lower_angle, 0);
}
