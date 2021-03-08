#include <stdio.h>

#include <boost/test/unit_test.hpp>

// For testing private functions
#define private public
#include "../posture_estimator.h"
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

