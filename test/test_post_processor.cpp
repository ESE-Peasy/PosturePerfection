#include <boost/test/unit_test.hpp>

#include "../src/iir.h"
#include "../src/post_processor.h"

bool check_untrustworthy(PostProcessing::Coordinate result) {
  return result.status == PostProcessing::Untrustworthy;
}

bool check_trustworthy(PostProcessing::Coordinate result) {
  return result.status == PostProcessing::Trustworthy;
}

BOOST_AUTO_TEST_CASE(LowConfidenceLabelledAsUntrustWorthy) {
  IIR::SmoothingSettings settings =
      IIR::SmoothingSettings{std::vector<std::vector<float>>{}};
  PostProcessing::PostProcessor post_proc =
      PostProcessing::PostProcessor(1.0, settings);

  Inference::InferenceResults dummy_input = Inference::InferenceResults{
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

  BOOST_TEST(check_untrustworthy(output.head_top));
  BOOST_TEST(check_untrustworthy(output.upper_neck));
  BOOST_TEST(check_untrustworthy(output.right_shoulder));
  BOOST_TEST(check_untrustworthy(output.right_elbow));
  BOOST_TEST(check_untrustworthy(output.right_wrist));
  BOOST_TEST(check_untrustworthy(output.thorax));
  BOOST_TEST(check_untrustworthy(output.left_shoulder));
  BOOST_TEST(check_untrustworthy(output.left_elbow));
  BOOST_TEST(check_untrustworthy(output.left_wrist));
  BOOST_TEST(check_untrustworthy(output.pelvis));
  BOOST_TEST(check_untrustworthy(output.right_hip));
  BOOST_TEST(check_untrustworthy(output.right_knee));
  BOOST_TEST(check_untrustworthy(output.right_ankle));
  BOOST_TEST(check_untrustworthy(output.left_hip));
  BOOST_TEST(check_untrustworthy(output.left_knee));
  BOOST_TEST(check_untrustworthy(output.left_ankle));
}

BOOST_AUTO_TEST_CASE(HighConfidenceLabelledAsTrustWorthy) {
  IIR::SmoothingSettings settings =
      IIR::SmoothingSettings{std::vector<std::vector<float>>{}};
  PostProcessing::PostProcessor post_proc =
      PostProcessing::PostProcessor(0.0, settings);

  Inference::InferenceResults dummy_input = Inference::InferenceResults{
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

  BOOST_TEST(check_trustworthy(output.head_top));
  BOOST_TEST(check_trustworthy(output.upper_neck));
  BOOST_TEST(check_trustworthy(output.right_shoulder));
  BOOST_TEST(check_trustworthy(output.right_elbow));
  BOOST_TEST(check_trustworthy(output.right_wrist));
  BOOST_TEST(check_trustworthy(output.thorax));
  BOOST_TEST(check_trustworthy(output.left_shoulder));
  BOOST_TEST(check_trustworthy(output.left_elbow));
  BOOST_TEST(check_trustworthy(output.left_wrist));
  BOOST_TEST(check_trustworthy(output.pelvis));
  BOOST_TEST(check_trustworthy(output.right_hip));
  BOOST_TEST(check_trustworthy(output.right_knee));
  BOOST_TEST(check_trustworthy(output.right_ankle));
  BOOST_TEST(check_trustworthy(output.left_hip));
  BOOST_TEST(check_trustworthy(output.left_knee));
  BOOST_TEST(check_trustworthy(output.left_ankle));
}
