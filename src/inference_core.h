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

class InferenceCore {
 private:
  std::unique_ptr<tflite::Interpreter> interpreter;
  size_t model_input_width;
  size_t model_input_height;
  size_t model_input_size;
  uint8_t model_input_channels = 3;


  Coordinate pixel_coord_to_Coordinate(uint32_t x, uint32_t y);

 public:
  InferenceCore(const char* model_filename, size_t model_input_width,
                size_t model_input_height);
  InferenceResults run(PreprocessedImage preprocessed_image);
};

#endif
