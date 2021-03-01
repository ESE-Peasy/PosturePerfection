#include <stdio.h>

#include <boost/test/unit_test.hpp>

// For testing private functions
#define private public
#include "../src/posture_estimator.h"
#undef private
#include <cmath>

PostureEstimating::Pose helper_ideal() {
  PostureEstimating::ConnectedJoint head = {
      PostureEstimating::Head,
      PostProcessing::Coordinate{1, 1, PostProcessing::Trustworthy}};
  PostureEstimating::ConnectedJoint neck = {
      PostureEstimating::Neck,
      PostProcessing::Coordinate{1, 1, PostProcessing::Trustworthy}};
  PostureEstimating::ConnectedJoint shoulder = {
      PostureEstimating::Shoulder,
      PostProcessing::Coordinate{1, 1, PostProcessing::Trustworthy}};
  PostureEstimating::ConnectedJoint hip = {
      PostureEstimating::Hip,
      PostProcessing::Coordinate{1, 1, PostProcessing::Trustworthy}};
  PostureEstimating::ConnectedJoint knee = {
      PostureEstimating::Knee,
      PostProcessing::Coordinate{1, 1, PostProcessing::Trustworthy}};
  PostureEstimating::ConnectedJoint foot = {
      PostureEstimating::Foot,
      PostProcessing::Coordinate{1, 1, PostProcessing::Trustworthy}};

  head.lower_connected_joint = &neck;
  head.lower_angle = M_PI / 2;

  neck.upper_angle = M_PI / 2;
  neck.upper_connected_joint = &head;
  neck.lower_connected_joint = &shoulder;
  neck.lower_angle = M_PI / 2;

  shoulder.upper_angle = M_PI / 2;
  shoulder.upper_connected_joint = &shoulder;
  shoulder.lower_connected_joint = &hip;
  shoulder.lower_angle = M_PI / 2;

  hip.upper_angle = M_PI / 2;
  hip.upper_connected_joint = &shoulder;
  hip.lower_connected_joint = &knee;
  hip.lower_angle = M_PI / 2;

  knee.upper_angle = M_PI / 2;
  knee.upper_connected_joint = &hip;
  knee.lower_connected_joint = &foot;
  knee.lower_angle = M_PI / 2;

  foot.upper_angle = M_PI / 2;
  foot.upper_connected_joint = &knee;

  PostureEstimating::Pose ideal = {&head, &neck, &shoulder, &hip, &knee, &foot};
  return ideal;
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
  PostureEstimating::Pose ideal = helper_ideal();
  BOOST_CHECK_EQUAL(ideal.neck->joint,
                    ideal.head->lower_connected_joint->joint);
}
