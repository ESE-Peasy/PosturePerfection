#include <create_op_resolver.h>
#include <interpreter_builder.h>
#include <model_builder.h>
#include <op_resolver.h>
#include <stderr_reporter.h>

#include <memory>

#include "posture_in.c"

enum BodyPart {
  head_top,
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
  left_ankle
};

typedef struct Result {
  float confidence;
  int x;
  int y;
} Result;

int main(int, char **) {
  tflite::StderrReporter error_reporter;

  auto model = tflite::FlatBufferModel::BuildFromFile(
      "../models/EfficientPoseRT_LITE.tflite", &error_reporter);

  auto resolver = tflite::CreateOpResolver();

  // auto interpreter_builder = ;

  std::unique_ptr<tflite::Interpreter> interpreter;

  if (tflite::InterpreterBuilder(*model, *resolver)(&interpreter) !=
      kTfLiteOk) {
    // Return failure.
    printf("Erorroor\n");
  }

  interpreter->AllocateTensors();
  auto input = &(interpreter->typed_input_tensor<float>(0)[0]);

  auto length = image.width * image.height * image.bytes_per_pixel;
  auto step = image.bytes_per_pixel;

  for (int i = 0; i < length; i += step) {
    const uint8_t *img = &(image.pixel_data[i]);
    input[i + 0] = img[0];
    input[i + 1] = img[1];
    input[i + 2] = img[2];
  }
  interpreter->Invoke();

  auto output = &(interpreter->typed_output_tensor<float>(0)[0]);

  Result results[16];

  for (int i = 0; i < length; i += 16) {
    // printf("[%d] ", i);
    for (int body_part = head_top; body_part <= left_ankle; body_part++) {
      auto out = output[i + body_part];
      // printf("%f ", out);
      if (out > results[body_part].confidence) {
        results[body_part] = {out, (i / 16) / image.width,
                              (i / 16) % image.height};
      }
    }
    // printf("\n");
  }

  for (int body_part = head_top; body_part <= left_ankle; body_part++) {
    auto result = results[body_part];
    printf("%d: %f @ (%d, %d)\n", body_part, result.confidence, result.x,
           result.y);
  }
  printf("The End\n");
}
