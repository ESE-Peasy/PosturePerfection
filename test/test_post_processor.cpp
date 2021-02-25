#define BOOST_TEST_MODULE PostProcessorTests
#include <boost/test/unit_test.hpp>

#include "post_processor.h"

BOOST_AUTO_TEST_CASE(PassTest) {
  float** coeffs = {{0, 0, 0, 0, 0, 0}};
  settings = PostProcessing::SmoothingSettings{coeffs, 1};
  post_proc = PostProcessing::PostProcessor(1.0, settings);
}
