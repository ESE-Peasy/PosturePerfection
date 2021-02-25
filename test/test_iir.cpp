#include <boost/test/unit_test.hpp>

#include "../src/iir.h"

BOOST_AUTO_TEST_CASE(NoFilteringWhenEmptySOSInputs) {
  float ** empty_sos_array = nullptr;
  IIR::SmoothingSettings settings = IIR::SmoothingSettings{empty_sos_array, 0};

  float input_data[] = {0.0, 0.0, 1.0, 0.0, -3.1};

  IIR::IIRFilter filter = IIR::IIRFilter(settings);

  for (int i = 0; i < sizeof(input_data)/sizeof(float); i++) {
    BOOST_CHECK_EQUAL(input_data[i], filter.run(input_data[i]));
  }

}
