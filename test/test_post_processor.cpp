#include <boost/test/unit_test.hpp>

#include "../src/iir.h"
#include "../src/post_processor.h"

BOOST_AUTO_TEST_CASE(PassTest) {
  float** coeffs = nullptr;
  IIR::SmoothingSettings settings = IIR::SmoothingSettings{coeffs, 0};
  PostProcessing::PostProcessor post_proc =
      PostProcessing::PostProcessor(1.0, settings);
}
