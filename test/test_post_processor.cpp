#include <boost/test/unit_test.hpp>

#include "../src/iir.h"
#include "../src/post_processor.h"

bool check_untrustworthy(PostProcessing::Coordinate result) {
  return result.status == PostProcessing::Untrustworthy;
}

bool check_trustworthy(PostProcessing::Coordinate result) {
  return result.status == PostProcessing::Trustworthy;
}

bool check_coords_match(PostProcessing::Coordinate result,
                        Inference::Coordinate input) {
  return result.x == input.x && result.y == input.y;
}

BOOST_AUTO_TEST_CASE(LowConfidenceLabelledAsUntrustWorthy) {
  IIR::SmoothingSettings settings =
      IIR::SmoothingSettings{std::vector<std::vector<float>>{}};
  PostProcessing::PostProcessor post_proc =
      PostProcessing::PostProcessor(1.0, settings);

  Inference::InferenceResults dummy_input;
  dummy_input.body_parts = {
      Inference::Coordinate{0, 0, 0.0}, Inference::Coordinate{0, 0, 0.1},
      Inference::Coordinate{0, 0, 0.2}, Inference::Coordinate{0, 0, 0.3},
      Inference::Coordinate{0, 0, 0.4}, Inference::Coordinate{0, 0, 0.5},
      Inference::Coordinate{0, 0, 0.6}, Inference::Coordinate{0, 0, 0.7},
      Inference::Coordinate{0, 0, 0.8}, Inference::Coordinate{0, 0, 0.7},
      Inference::Coordinate{0, 0, 0.6}, Inference::Coordinate{0, 0, 0.5},
      Inference::Coordinate{0, 0, 0.4}, Inference::Coordinate{0, 0, 0.3},
      Inference::Coordinate{0, 0, 0.2}, Inference::Coordinate{0, 0, 0.1},
  };

  PostProcessing::ProcessedResults output = post_proc.run(dummy_input);
  for (auto body_part : output.body_parts) {
    BOOST_TEST(check_untrustworthy(body_part));
  }
}

BOOST_AUTO_TEST_CASE(HighConfidenceLabelledAsTrustWorthy) {
  IIR::SmoothingSettings settings =
      IIR::SmoothingSettings{std::vector<std::vector<float>>{}};
  PostProcessing::PostProcessor post_proc =
      PostProcessing::PostProcessor(0.0, settings);

  Inference::InferenceResults dummy_input;
  dummy_input.body_parts = {
      Inference::Coordinate{0, 0, 0.1}, Inference::Coordinate{0, 0, 0.1},
      Inference::Coordinate{0, 0, 0.2}, Inference::Coordinate{0, 0, 0.3},
      Inference::Coordinate{0, 0, 0.4}, Inference::Coordinate{0, 0, 0.5},
      Inference::Coordinate{0, 0, 0.6}, Inference::Coordinate{0, 0, 0.7},
      Inference::Coordinate{0, 0, 0.8}, Inference::Coordinate{0, 0, 0.7},
      Inference::Coordinate{0, 0, 0.6}, Inference::Coordinate{0, 0, 0.5},
      Inference::Coordinate{0, 0, 0.4}, Inference::Coordinate{0, 0, 0.3},
      Inference::Coordinate{0, 0, 0.2}, Inference::Coordinate{0, 0, 0.1},
  };

  PostProcessing::ProcessedResults output = post_proc.run(dummy_input);

  for (auto body_part : output.body_parts) {
    BOOST_TEST(check_trustworthy(body_part));
  }
}

BOOST_AUTO_TEST_CASE(CoordinatesPassedThroughWhenNotFiltered) {
  IIR::SmoothingSettings settings =
      IIR::SmoothingSettings{std::vector<std::vector<float>>{}};
  PostProcessing::PostProcessor post_proc =
      PostProcessing::PostProcessor(0.0, settings);

  Inference::InferenceResults dummy_input;
  dummy_input.body_parts = {
      Inference::Coordinate{0.1, 0.2, 0},
      Inference::Coordinate{0.17, 0.18, 0},
      Inference::Coordinate{0.3, 0.4, 0},
      Inference::Coordinate{0.19, 0.201, 0},
      Inference::Coordinate{0.5, 0.6, 0},
      Inference::Coordinate{0.21, 0.22, 0},
      Inference::Coordinate{0.7, 0.8, 0},
      Inference::Coordinate{0.23, 0.24, 0},
      Inference::Coordinate{0.9, 0.101, 0},
      Inference::Coordinate{0.25, 0.26, 0},
      Inference::Coordinate{0.11, 0.12, 0},
      Inference::Coordinate{0.27, 0.28, 0},
      Inference::Coordinate{0.13, 0.14, 0},
      Inference::Coordinate{0.29, 0.301, 0},
      Inference::Coordinate{0.15, 0.16, 0},
      Inference::Coordinate{0.31, 0.32, 0},
  };

  PostProcessing::ProcessedResults output = post_proc.run(dummy_input);

  int body_part_index = BodyPartMin;
  for (auto body_part : output.body_parts) {
    BOOST_TEST(
        check_coords_match(body_part, dummy_input.body_parts[body_part_index]));
    body_part_index++;
  }
}
