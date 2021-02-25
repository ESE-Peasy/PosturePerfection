#define BOOST_TEST_MODULE EstimatorTests
#include <boost/test/unit_test.hpp>

//For testing private functions
#define private public
#include "posture_estimator.h"
#undef private


#include "post_processor.h"


BOOST_AUTO_TEST_CASE(LinesAngleCorrect){
    PostProcessing::Coordinate p1;
    PostProcessing::Coordinate p2;
    p1.x = 1 ;
    p1.y = 1 ;

    p2.x=4 ;
    p2.y=4 ;

    PostureEstimating::PostureEstimator e;

    BOOST_CHECK_EQUAL(45, e.getLineAngle(p1, p2));
};