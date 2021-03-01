#include <stdio.h>

#include <boost/test/unit_test.hpp>

// For testing private functions
#define private public
#include "../src/posture_estimator.h"
#undef private
#include <cmath>

PostureEstimating::Pose helper_create_pose() {
  PostureEstimating::ConnectedJoint *head =
      (PostureEstimating::ConnectedJoint *)malloc(
          (sizeof(PostureEstimating::ConnectedJoint)));
  PostureEstimating::ConnectedJoint *neck =
      (PostureEstimating::ConnectedJoint *)malloc(
          (sizeof(PostureEstimating::ConnectedJoint)));
  PostureEstimating::ConnectedJoint *shoulder =
      (PostureEstimating::ConnectedJoint *)malloc(
          (sizeof(PostureEstimating::ConnectedJoint)));
  PostureEstimating::ConnectedJoint *hip =
      (PostureEstimating::ConnectedJoint *)malloc(
          (sizeof(PostureEstimating::ConnectedJoint)));
  PostureEstimating::ConnectedJoint *knee =
      (PostureEstimating::ConnectedJoint *)malloc(
          (sizeof(PostureEstimating::ConnectedJoint)));
  PostureEstimating::ConnectedJoint *foot =
      (PostureEstimating::ConnectedJoint *)malloc(
          (sizeof(PostureEstimating::ConnectedJoint)));

  head->joint = PostureEstimating::Head;
  head->coord = PostProcessing::Coordinate{1, 1, PostProcessing::Trustworthy};
  head->lower_connected_joint = neck;
  head->lower_angle = -M_PI;

  neck->joint = PostureEstimating::Neck;
  neck->coord = PostProcessing::Coordinate{1, 1, PostProcessing::Trustworthy};
  neck->upper_angle = 0;
  neck->upper_connected_joint = head;
  neck->lower_connected_joint = shoulder;
  neck->lower_angle = -M_PI;

  shoulder->joint = PostureEstimating::Shoulder;
  shoulder->coord =
      PostProcessing::Coordinate{1, 1, PostProcessing::Trustworthy};
  shoulder->upper_angle = 0;
  shoulder->upper_connected_joint = neck;
  shoulder->lower_connected_joint = hip;
  shoulder->lower_angle = -M_PI;

  hip->joint = PostureEstimating::Hip;
  hip->coord = PostProcessing::Coordinate{1, 1, PostProcessing::Trustworthy};
  hip->upper_angle = 0;
  hip->upper_connected_joint = shoulder;
  hip->lower_connected_joint = knee;
  hip->lower_angle = -M_PI;

  knee->joint = PostureEstimating::Knee;
  knee->coord = PostProcessing::Coordinate{1, 1, PostProcessing::Trustworthy};
  knee->upper_angle = 0;
  knee->upper_connected_joint = hip;
  knee->lower_connected_joint = foot;
  knee->lower_angle = -M_PI;

  foot->joint = PostureEstimating::Foot;
  foot->coord = PostProcessing::Coordinate{1, 1, PostProcessing::Trustworthy};
  foot->upper_angle = 0;
  foot->upper_connected_joint = knee;

  PostureEstimating::Pose ideal = {head, neck, shoulder, hip, knee, foot};
  return ideal;
}

void helper_destroy_pose(PostureEstimating::Pose p) {
  free(p.head);
  free(p.neck);
  free(p.shoulder);
  free(p.hip);
  free(p.knee);
  free(p.foot);
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

  e.current_pose.head->lower_angle = -M_PI / 2;
  e.current_pose.neck->upper_angle = M_PI / 2;

  e.calculatePoseChanges();

  BOOST_CHECK_EQUAL(e.pose_changes.foot->upper_angle, 0);
  BOOST_CHECK_CLOSE(e.pose_changes.head->lower_angle, M_PI / 2, 0.00001);
  BOOST_CHECK_CLOSE(e.pose_changes.neck->upper_angle, -M_PI / 2, 0.00001);
  BOOST_CHECK_EQUAL(e.pose_changes.neck->lower_angle, 0);
  BOOST_CHECK_EQUAL(e.pose_changes.shoulder->upper_angle, 0);
  BOOST_CHECK_EQUAL(e.pose_changes.shoulder->lower_angle, 0);
  BOOST_CHECK_EQUAL(e.pose_changes.hip->upper_angle, 0);
  BOOST_CHECK_EQUAL(e.pose_changes.hip->lower_angle, 0);
  BOOST_CHECK_EQUAL(e.pose_changes.knee->upper_angle, 0);
  BOOST_CHECK_EQUAL(e.pose_changes.knee->lower_angle, 0);
  BOOST_CHECK_EQUAL(e.pose_changes.foot->upper_angle, 0);
  BOOST_CHECK_EQUAL(e.pose_changes.foot->lower_angle, 0);
}
