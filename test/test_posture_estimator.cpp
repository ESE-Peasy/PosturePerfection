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

  p.joints[PostureEstimating::Neck]->coord =
      PostProcessing::Coordinate{2, 1, PostProcessing::Trustworthy};
  p.joints[PostureEstimating::Neck]->lower_angle = -M_PI;

  p.joints[PostureEstimating::Shoulder]->coord =
      PostProcessing::Coordinate{3, 1, PostProcessing::Trustworthy};
  p.joints[PostureEstimating::Shoulder]->lower_angle = -M_PI;

  p.joints[PostureEstimating::Hip]->coord =
      PostProcessing::Coordinate{1, 1, PostProcessing::Trustworthy};
  p.joints[PostureEstimating::Hip]->lower_angle = -M_PI;

  p.joints[PostureEstimating::Knee]->coord =
      PostProcessing::Coordinate{1, 1, PostProcessing::Trustworthy};
  p.joints[PostureEstimating::Knee]->lower_angle = -M_PI;

  p.joints[PostureEstimating::Foot]->coord =
      PostProcessing::Coordinate{1, 1, PostProcessing::Trustworthy};

  return p;
}

void helper_destroy_pose(PostureEstimating::Pose p) {
  free(p.joints[PostureEstimating::Head]);
  free(p.joints[PostureEstimating::Neck]);
  free(p.joints[PostureEstimating::Shoulder]);
  free(p.joints[PostureEstimating::Hip]);
  free(p.joints[PostureEstimating::Knee]);
  free(p.joints[PostureEstimating::Foot]);
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
  BOOST_CHECK_EQUAL(
      e.pose_changes.joints[PostureEstimating::Shoulder]->upper_angle, 0);
  BOOST_CHECK_EQUAL(
      e.pose_changes.joints[PostureEstimating::Shoulder]->lower_angle, 0);
  BOOST_CHECK_EQUAL(e.pose_changes.joints[PostureEstimating::Hip]->upper_angle,
                    0);
  BOOST_CHECK_EQUAL(e.pose_changes.joints[PostureEstimating::Hip]->lower_angle,
                    0);
  BOOST_CHECK_EQUAL(e.pose_changes.joints[PostureEstimating::Knee]->upper_angle,
                    0);
  BOOST_CHECK_EQUAL(e.pose_changes.joints[PostureEstimating::Knee]->lower_angle,
                    0);
  BOOST_CHECK_EQUAL(e.pose_changes.joints[PostureEstimating::Foot]->upper_angle,
                    0);
  BOOST_CHECK_EQUAL(e.pose_changes.joints[PostureEstimating::Foot]->lower_angle,
                    0);
  helper_destroy_pose(e.current_pose);
  helper_destroy_pose(e.ideal_pose);
}
