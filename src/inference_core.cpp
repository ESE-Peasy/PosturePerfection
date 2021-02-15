#include "inference_core.h"

#include <create_op_resolver.h>
#include <model_builder.h>
#include <op_resolver.h>
#include <stderr_reporter.h>
#include <stdio.h>

#include <memory>

enum BodyPart {
  head_top,
  BodyPart_MIN = head_top,
  upper_neck,
  right_shoulder,
  right_elbow,
  right_wrist,
  thorax,
  left_shoulder,
  left_elbow,
  left_wrist,
  pelvis,
  right_hip,
  right_knee,
  right_ankle,
  left_hip,
  left_knee,
  left_ankle,
  BodyPart_MAX = left_ankle
};

typedef struct Result {
  float confidence;
  size_t x;
  size_t y;
} Result;

InferenceCore::InferenceCore(const char* model_filename,
                             size_t model_input_width,
                             size_t model_input_height) {
  this->model_input_width = model_input_width;
  this->model_input_height = model_input_height;
  this->model_input_size = model_input_width * model_input_height;

  tflite::StderrReporter error_reporter;

  // Load the model
  auto model =
      tflite::FlatBufferModel::BuildFromFile(model_filename, &error_reporter);

  auto resolver = tflite::CreateOpResolver();

  // Start the interpreter
  if (tflite::InterpreterBuilder(*model, *resolver)(&interpreter) !=
      kTfLiteOk) {
    // Return failure.
    fprintf(stderr, "Could not start interpreter\n");
    exit(EXIT_FAILURE);
  }
}

Coordinate InferenceCore::pixel_coord_to_Coordinate(uint32_t x, uint32_t y) {
  return Coordinate{(float)x / (float)this->model_input_width,
                    (float)y / (float)this->model_input_height};
}

InferenceResults InferenceCore::run(PreprocessedImage preprocessed_image) {
  // Initialise and get pointer to input
  interpreter->AllocateTensors();
  auto input = interpreter->typed_input_tensor<float>(0);

  size_t size = this->model_input_size;

  // Copy the image to the input
  memcpy(
      input, preprocessed_image.image,
      size * this->model_input_channels * sizeof(preprocessed_image.image[0]));

  // Run the model
  interpreter->Invoke();

  // Get pointer to output
  auto output = interpreter->typed_output_tensor<float>(0);

  Result results[BodyPart_MAX + 1];
  memset(results, 0, sizeof(results));

  size_t step = BodyPart_MAX + 1;
  size_t width = preprocessed_image.width;
  size_t height = preprocessed_image.height;

  // Go through output to find where largest confidence for each body part is
  // Step through each pixel
  for (uint32_t i = 0; i < size * step; i += step) {
    // Look at predictions for each body part in current pixel
    for (int body_part = BodyPart_MIN; body_part <= BodyPart_MAX; body_part++) {
      float out = output[i + body_part];
      if (out > results[body_part].confidence) {
        results[body_part] = {out, (i / step) % width, (i / step) / height};
      }
    }
  }

  InferenceResults results_out = {
      pixel_coord_to_Coordinate(results[head_top].x, results[head_top].y),
      pixel_coord_to_Coordinate(results[upper_neck].x, results[upper_neck].y),
      pixel_coord_to_Coordinate(results[right_shoulder].x,
                                results[right_shoulder].y),
      pixel_coord_to_Coordinate(results[right_elbow].x, results[right_elbow].y),
      pixel_coord_to_Coordinate(results[right_wrist].x, results[right_wrist].y),
      pixel_coord_to_Coordinate(results[thorax].x, results[thorax].y),
      pixel_coord_to_Coordinate(results[left_shoulder].x,
                                results[left_shoulder].y),
      pixel_coord_to_Coordinate(results[left_elbow].x, results[left_elbow].y),
      pixel_coord_to_Coordinate(results[left_wrist].x, results[left_wrist].y),
      pixel_coord_to_Coordinate(results[pelvis].x, results[pelvis].y),
      pixel_coord_to_Coordinate(results[right_hip].x, results[right_hip].y),
      pixel_coord_to_Coordinate(results[right_knee].x, results[right_knee].y),
      pixel_coord_to_Coordinate(results[right_ankle].x, results[right_ankle].y),
      pixel_coord_to_Coordinate(results[left_hip].x, results[left_hip].y),
      pixel_coord_to_Coordinate(results[left_knee].x, results[left_knee].y),
      pixel_coord_to_Coordinate(results[left_ankle].x, results[left_ankle].y)};

  return results_out;
}

int main(int argc, char const *argv[])
{
  // Dummy main for compiling
  return 0;
}
