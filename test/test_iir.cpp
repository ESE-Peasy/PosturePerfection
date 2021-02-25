#include <boost/test/unit_test.hpp>

#include "../src/iir.h"

BOOST_AUTO_TEST_CASE(NoFilteringWhenEmptySOSInputs) {
  IIR::SmoothingSettings settings =
      IIR::SmoothingSettings{std::vector<std::vector<float>>{}};

  float input_data[] = {0.0, 0.0, 1.0, 0.0, -3.1};

  IIR::IIRFilter filter = IIR::IIRFilter(settings);

  for (int i = 0; i < sizeof(input_data) / sizeof(float); i++) {
    BOOST_CHECK_EQUAL(input_data[i], filter.run(input_data[i]));
  }
}

BOOST_AUTO_TEST_CASE(SmoothHighFreqChanges) {
  std::vector<std::vector<float>> coeffs{
      {8.04235642e-07F, 1.60847128e-06F, 8.04235642e-07F, 1.00000000e+00F,
       -8.81618592e-01F, 0.00000000e+00F},
      {1.00000000e+00F, 2.00000000e+00F, 1.00000000e+00F, 1.00000000e+00F,
       -1.80155740e+00F, 8.15876124e-01F},
      {1.00000000e+00F, 1.00000000e+00F, 0.00000000e+00F, 1.00000000e+00F,
       -1.91024541e+00F, 9.25427983e-01F}};

  IIR::SmoothingSettings settings = IIR::SmoothingSettings{coeffs};

  float input_data[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                        1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

  IIR::IIRFilter filter = IIR::IIRFilter(settings);

  for (int i = 0; i < sizeof(input_data) / sizeof(float); i++) {
    auto result = filter.run(input_data[i]);
    // Check result stays "small"
    BOOST_CHECK_LT(result, 0.1);
  }
}

BOOST_AUTO_TEST_CASE(SlowChangesPass) {
  std::vector<std::vector<float>> coeffs{
      {8.04235642e-07F, 1.60847128e-06F, 8.04235642e-07F, 1.00000000e+00F,
       -8.81618592e-01F, 0.00000000e+00F},
      {1.00000000e+00F, 2.00000000e+00F, 1.00000000e+00F, 1.00000000e+00F,
       -1.80155740e+00F, 8.15876124e-01F},
      {1.00000000e+00F, 1.00000000e+00F, 0.00000000e+00F, 1.00000000e+00F,
       -1.91024541e+00F, 9.25427983e-01F}};

  IIR::SmoothingSettings settings = IIR::SmoothingSettings{coeffs};

  float input_data[] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9,
                        1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.9, 0.8, 0.7, 0.6,
                        0.5, 0.4, 0.3, 0.2, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

  IIR::IIRFilter filter = IIR::IIRFilter(settings);

  float sum = 0;
  for (int i = 0; i < sizeof(input_data) / sizeof(float); i++) {
    sum += filter.run(input_data[i]);
  }
  BOOST_CHECK_GE(sum, 5.0);
}
