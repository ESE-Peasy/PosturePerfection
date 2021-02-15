/**
 * @file inference_core.h
 * @brief Wrapper API for the TensorFlow Lite model
 *
 */

#ifndef INFERENCE_CORE_H
#define INFERENCE_CORE_H

#include <interpreter_builder.h>
#include <stdint.h>

typedef struct Coordinate {
  float x;
  float y;
} Coordinate;

typedef struct InferenceResults {
  Coordinate head_top;
  Coordinate upper_neck;
  Coordinate right_shoulder;
  Coordinate right_elbow;
  Coordinate right_wrist;
  Coordinate thorax;
  Coordinate left_shoulder;
  Coordinate left_elbow;
  Coordinate left_wrist;
  Coordinate pelvis;
  Coordinate right_hip;
  Coordinate right_knee;
  Coordinate right_ankle;
  Coordinate left_hip;
  Coordinate left_knee;
  Coordinate left_ankle;
} InferenceResults;

typedef struct PreprocessedImage {
  float* image;
  size_t width;
  size_t height;
  uint8_t channels = 3;
} PreprocessedImage;

/**
 * @brief A wrapper to make running inference easier
 *
 * This class hides some of the complexity of the underlying TensorFlow Lite C++
 * API. It nonetheless expects preprocessed images. This means images will need
 * to be resized to the model input dimensions and normalised from `uint8_t`
 * values to `float`s in the interval [-1..1] before being run through a
 * `InferenceCore` object.
 *
 */
class InferenceCore {
 private:
  std::unique_ptr<tflite::Interpreter> interpreter;
  size_t model_input_width;
  size_t model_input_height;
  size_t model_input_size;
  uint8_t model_input_channels = 3;

  Coordinate pixel_coord_to_Coordinate(uint32_t x, uint32_t y);

 public:
  /**
   * @brief Construct a new Inference Core object
   *
   * @param model_filename Path to the TensorFlow Lite model
   * @param model_input_width Width of the input to the model
   * @param model_input_height Height of the input to the model
   */
  InferenceCore(const char* model_filename, size_t model_input_width,
                size_t model_input_height);

  /**
   * @brief Run a pre-processed image through the loaded model
   *
   * @param preprocessed_image An image that has been resized to the model input
   * dimensions and normalised from `uint8_t` values to `float`s in the interval
   * [-1..1] before being passed to this function
   * @return InferenceResults
   */
  InferenceResults run(PreprocessedImage preprocessed_image);
};

#endif
