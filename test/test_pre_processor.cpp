#include <boost/test/unit_test.hpp>

#include "../src/pre_processor.h"
#include "opencv2/opencv.hpp"

#define MODEL_INPUT_X 224
#define MODEL_INPUT_Y 224
#define MODEL_NUM_CHANNELS 3

bool check_normalised(float value) { return value <= 1.0 && value >= -1.0; }

BOOST_AUTO_TEST_CASE(ImageNormalised) {
  cv::Mat image = cv::imread("../../test/test_image.jpg");

  PreProcessing::PreProcessor pre_proc =
      PreProcessing::PreProcessor(MODEL_INPUT_X, MODEL_INPUT_Y);

  PreProcessing::PreProcessedImage pre_proc_img = pre_proc.run(image);

  // Check image has been normalised to range [-1..1]
  for (int i = 0; i < MODEL_INPUT_X * MODEL_INPUT_Y * MODEL_NUM_CHANNELS;
       i += 1) {
    BOOST_TEST(check_normalised(pre_proc_img.image[i]));
  }
}

BOOST_AUTO_TEST_CASE(RBChannelsSwitched) {
  cv::Mat image = cv::imread("../../test/test_image.jpg");

  PreProcessing::PreProcessor pre_proc =
      PreProcessing::PreProcessor(MODEL_INPUT_X, MODEL_INPUT_Y);

  PreProcessing::PreProcessedImage pre_proc_img = pre_proc.run(image);

  // Manually resize and normalise to compare R and B channels
  cv::resize(image, image, cv::Size(MODEL_INPUT_X, MODEL_INPUT_Y));
  uint8_t* resized_image = image.data;

  float* non_switched_img = reinterpret_cast<float*>(malloc(
      MODEL_INPUT_X * MODEL_INPUT_Y * MODEL_NUM_CHANNELS * sizeof(float)));

  int size = MODEL_INPUT_X * MODEL_INPUT_Y;
  int step = MODEL_NUM_CHANNELS;

  for (int i = 0; i < size * step; i += step) {
    // Don't switch R and B channels
    uint8_t* img = &(resized_image[i]);
    non_switched_img[i + 0] = img[0] / 127.5 - 1;
    non_switched_img[i + 1] = img[1] / 127.5 - 1;
    non_switched_img[i + 2] = img[2] / 127.5 - 1;
  }

  // Check channels have been switched
  for (int i = 0; i < size * step; i += step) {
    BOOST_TEST(pre_proc_img.image[i + 0], non_switched_img[i + 2]);
    BOOST_TEST(pre_proc_img.image[i + 1], non_switched_img[i + 1]);
    BOOST_TEST(pre_proc_img.image[i + 2], non_switched_img[i + 0]);
  }
}
