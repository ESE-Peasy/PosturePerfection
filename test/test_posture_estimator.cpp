#include <stdio.h>

#include <boost/test/output_test_stream.hpp>
#include <boost/test/unit_test.hpp>

// For testing private functions
#define private public
#include "../src/posture_estimator.h"
#undef private

BOOST_AUTO_TEST_CASE(LinesAngleCorrect) {
  PostProcessing::Coordinate p1;
  PostProcessing::Coordinate p2;
  p1.x = 1;
  p1.y = 1;

  p2.x = 4;
  p2.y = 4;

  PostureEstimating::PostureEstimator e;
  boost::test_tools::output_test_stream output;
  float angle = e.getLineAngle(p1, p2);
  BOOST_TEST((angle - ((45.0 / 180.0) * M_PI)) <
             0.0000001);  // Work to an accuracy as M_PI is more digits than
                          // atan returns
};
