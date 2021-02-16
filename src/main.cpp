#include "posture_in.c"  // TODO Remove

#include "inference_core.h"

#define MODEL_INPUT_X 224
#define MODEL_INPUT_Y 224

int main(int argc, char const *argv[]) {
  float preprocessed_image[224 * 224 * 3 + 1];

  int size = MODEL_INPUT_X * MODEL_INPUT_Y;
  int step = 3;

  for (int i = 0; i < size * step; i += step) {
    const uint8_t *img = &(image.pixel_data[i]);
    // Scale to [-1..1]
    preprocessed_image[i + 0] = img[0] / 127.5 - 1;
    preprocessed_image[i + 1] = img[1] / 127.5 - 1;
    preprocessed_image[i + 2] = img[2] / 127.5 - 1;
  }

  InferenceCore core("../models/EfficientPoseRT_LITE.tflite", MODEL_INPUT_X,
                     MODEL_INPUT_Y);

  InferenceResults results = core.run(PreprocessedImage{preprocessed_image});

  printf("%f, %f\n", results.head_top.x, results.head_top.y);
  printf("%f, %f\n", results.upper_neck.x, results.upper_neck.y);
  printf("%f, %f\n", results.right_shoulder.x, results.right_shoulder.y);
  printf("%f, %f\n", results.right_elbow.x, results.right_elbow.y);
  printf("%f, %f\n", results.right_wrist.x, results.right_wrist.y);
  printf("%f, %f\n", results.thorax.x, results.thorax.y);
  printf("%f, %f\n", results.left_shoulder.x, results.left_shoulder.y);
  printf("%f, %f\n", results.left_elbow.x, results.left_elbow.y);
  printf("%f, %f\n", results.left_wrist.x, results.left_wrist.y);
  printf("%f, %f\n", results.pelvis.x, results.pelvis.y);
  printf("%f, %f\n", results.right_hip.x, results.right_hip.y);
  printf("%f, %f\n", results.right_knee.x, results.right_knee.y);
  printf("%f, %f\n", results.right_ankle.x, results.right_ankle.y);
  printf("%f, %f\n", results.left_hip.x, results.left_hip.y);
  printf("%f, %f\n", results.left_knee.x, results.left_knee.y);
  printf("%f, %f\n", results.left_ankle.x, results.left_ankle.y);
}
