#include <stdio.h>

#include <boost/test/unit_test.hpp>

// For testing private functions
#define private public
#include "../src/posture_estimator.h"
#undef private

BOOST_AUTO_TEST_CASE(LinesAngleCorrect) {
  PostProcessing::Coordinate p = {1, 1, PostProcessing::Trustworthy};
  PostProcessing::Coordinate p1 = {3, 4, PostProcessing::Trustworthy};

  PostureEstimating::PostureEstimator e;
  float angle = e.getLineAngle(p, p1);

  BOOST_CHECK_CLOSE(angle, (0.5880026035),
                    0.00001);  // Rounding due to using float
};

BOOST_AUTO_TEST_CASE(LinesAngleInAllQuads) {
  PostProcessing::Coordinate p = {1, 1, PostProcessing::Trustworthy};
  PostProcessing::Coordinate p1 = {2, 2, PostProcessing::Trustworthy};
  PostProcessing::Coordinate p2 = {-2, 2, PostProcessing::Trustworthy};
  PostProcessing::Coordinate p3 = {-2, -2, PostProcessing::Trustworthy};
  PostProcessing::Coordinate p4 = {2, -2, PostProcessing::Trustworthy};

  PostureEstimating::PostureEstimator e;
  float angleQ1 = e.getLineAngle(p, p1);
  float angleQ2 = e.getLineAngle(p, p2);
  float angleQ3 = e.getLineAngle(p, p3);
  float angleQ4 = e.getLineAngle(p1, p4);

  BOOST_CHECK_CLOSE(angleQ1, ((45.0 / 180.0) * M_PI),
                    0.00001);  // Rounding due to using float
  // BOOST_CHECK_CLOSE(angleQ4, ((135.0 / 180.0) * M_PI),
  //                   0.00001);  // Rounding due to using float
  BOOST_TEST(angleQ2 == (-45.0 / 180.0) * M_PI);
  // 0.00001);  // Rounding due to using float

  // BOOST_CHECK_CLOSE(angleQ3, ((-135.0 / 180.0) * M_PI),
  //                   0.00001);  // Rounding due to using float
};

BOOST_AUTO_TEST_CASE(LinesAngleSlopeZero) {
  PostProcessing::Coordinate p1 = {1, 1, PostProcessing::Trustworthy};
  PostProcessing::Coordinate p2 = {4, 1, PostProcessing::Trustworthy};
  PostProcessing::Coordinate p3 = {1, 4, PostProcessing::Trustworthy};

  PostureEstimating::PostureEstimator e;

  BOOST_CHECK_CLOSE(e.getLineAngle(p1, p2), M_PI / 2,
                    0.00001);  // Rounding due to using float
  BOOST_CHECK_CLOSE(e.getLineAngle(p1, p3), M_PI / 2,
                    0.00001);  // Rounding due to using float
};
