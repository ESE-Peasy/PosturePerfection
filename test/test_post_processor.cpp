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

float mean_of_coords_x(Inference::Coordinate input_left,
                       Inference::Coordinate input_right) {
  return (input_left.x + input_right.x) / 2;
}

float mean_of_coords_y(Inference::Coordinate input_left,
                       Inference::Coordinate input_right) {
  return (input_left.y + input_right.y) / 2;
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
      Inference::Coordinate{0.1, 0.2, 1},
      Inference::Coordinate{0.17, 0.18, 1},
      Inference::Coordinate{0.3, 0.4, 1},
      Inference::Coordinate{0.19, 0.201, 1},
      Inference::Coordinate{0.5, 0.6, 1},
      Inference::Coordinate{0.21, 0.22, 1},
      Inference::Coordinate{0.7, 0.8, 0},
      Inference::Coordinate{0.23, 0.24, 0},
      Inference::Coordinate{0.9, 0.101, 0},
      Inference::Coordinate{0.25, 0.26, 0},
      Inference::Coordinate{0.11, 0.12, 1},
      Inference::Coordinate{0.27, 0.28, 1},
      Inference::Coordinate{0.13, 0.14, 1},
      Inference::Coordinate{0.29, 0.301, 0},
      Inference::Coordinate{0.15, 0.16, 0},
      Inference::Coordinate{0.31, 0.32, 0},
  };

  PostProcessing::ProcessedResults output = post_proc.run(dummy_input);

  check_coords_match(output.body_parts[Head], dummy_input.body_parts[head_top]);
  check_coords_match(output.body_parts[Neck],
                     dummy_input.body_parts[upper_neck]);
  check_coords_match(output.body_parts[Shoulder],
                     dummy_input.body_parts[right_shoulder]);
  check_coords_match(output.body_parts[Hip], dummy_input.body_parts[right_hip]);
  check_coords_match(output.body_parts[Knee],
                     dummy_input.body_parts[right_knee]);
  check_coords_match(output.body_parts[Foot],
                     dummy_input.body_parts[right_ankle]);
}

BOOST_AUTO_TEST_CASE(AverageCoordinatesWhenBothTrustworthy) {
  IIR::SmoothingSettings settings =
      IIR::SmoothingSettings{std::vector<std::vector<float>>{}};
  PostProcessing::PostProcessor post_proc =
      PostProcessing::PostProcessor(0.0, settings);

  Inference::InferenceResults dummy_input;
  dummy_input.body_parts = {
      Inference::Coordinate{0, 0, 1.0},
      Inference::Coordinate{0, 0, 1.0},
      Inference::Coordinate{0, 0, 1.0},  // Right shoulder
      Inference::Coordinate{0, 0, 1.0},
      Inference::Coordinate{0, 0, 1.0},
      Inference::Coordinate{0, 0, 1.0},
      Inference::Coordinate{1, 1, 1.0},  // Left shoulder
      Inference::Coordinate{0, 0, 1.0},
      Inference::Coordinate{0, 0, 1.0},
      Inference::Coordinate{0, 0, 1.0},  // Pelvis
      Inference::Coordinate{0, 0, 1.0},  // Right hip
      Inference::Coordinate{0, 0, 1.0},
      Inference::Coordinate{0, 0, 1.0},
      Inference::Coordinate{0.5, 0.5, 1.0},  // Left hip
      Inference::Coordinate{0, 0, 1.0},
      Inference::Coordinate{0, 0, 1.0},
  };

  PostProcessing::ProcessedResults output = post_proc.run(dummy_input);

  // Check shoulder mean works
  float x = mean_of_coords_x(dummy_input.body_parts[left_shoulder],
                             dummy_input.body_parts[right_shoulder]);
  float y = mean_of_coords_y(dummy_input.body_parts[left_shoulder],
                             dummy_input.body_parts[right_shoulder]);
  BOOST_CHECK_CLOSE(x, output.body_parts[Shoulder].x, 0.1);
  BOOST_CHECK_CLOSE(y, output.body_parts[Shoulder].y, 0.1);

  // Check hip mean works
  x = mean_of_coords_x(dummy_input.body_parts[left_hip],
                       dummy_input.body_parts[right_hip]);
  x = (dummy_input.body_parts[pelvis].x + x) / 2;
  y = mean_of_coords_y(dummy_input.body_parts[left_hip],
                       dummy_input.body_parts[right_hip]);
  y = (dummy_input.body_parts[pelvis].y + y) / 2;
  BOOST_CHECK_CLOSE(x, output.body_parts[Hip].x, 0.1);
  BOOST_CHECK_CLOSE(y, output.body_parts[Hip].y, 0.1);
}

BOOST_AUTO_TEST_CASE(DontAverageCoordinatesWhenNotBothTrustworthy) {
  IIR::SmoothingSettings settings =
      IIR::SmoothingSettings{std::vector<std::vector<float>>{}};
  PostProcessing::PostProcessor post_proc =
      PostProcessing::PostProcessor(0.0, settings);

  Inference::InferenceResults dummy_input;
  dummy_input.body_parts = {
      Inference::Coordinate{0, 0, 1.0},
      Inference::Coordinate{0, 0, 1.0},
      Inference::Coordinate{0, 0, 1.0},  // Right shoulder
      Inference::Coordinate{0, 0, 1.0},
      Inference::Coordinate{0, 0, 1.0},
      Inference::Coordinate{0, 0, 1.0},
      Inference::Coordinate{1, 1, 0.0},  // Left shoulder
      Inference::Coordinate{0, 0, 1.0},
      Inference::Coordinate{0, 0, 1.0},
      Inference::Coordinate{0, 0, 0.0},  // Pelvis
      Inference::Coordinate{0, 0, 0.0},  // Right hip
      Inference::Coordinate{0, 0, 1.0},
      Inference::Coordinate{0, 0, 1.0},
      Inference::Coordinate{0.5, 0.5, 1.0},  // Left hip
      Inference::Coordinate{0, 0, 1.0},
      Inference::Coordinate{0, 0, 1.0},
  };

  PostProcessing::ProcessedResults output = post_proc.run(dummy_input);

  // Check shoulder mean works
  BOOST_CHECK_CLOSE(dummy_input.body_parts[right_shoulder].x,
                    output.body_parts[Shoulder].x, 0.1);
  BOOST_CHECK_CLOSE(dummy_input.body_parts[right_shoulder].y,
                    output.body_parts[Shoulder].y, 0.1);

  // Check hip mean works
  BOOST_CHECK_CLOSE(dummy_input.body_parts[left_hip].x,
                    output.body_parts[Hip].x, 0.1);
  BOOST_CHECK_CLOSE(dummy_input.body_parts[left_hip].y,
                    output.body_parts[Hip].y, 0.1);
}
