#include <stderr_reporter.h>
#include <model_builder.h>
#include <op_resolver.h>
#include <create_op_resolver.h>
#include <interpreter_builder.h>

int main(int, char**) {
  tflite::StderrReporter error_reporter;

  auto model = tflite::FlatBufferModel::BuildFromFile(
      "../models/EfficientPoseRT_LITE.tflite", &error_reporter);

  auto resolver = tflite::CreateOpResolver();

  tflite::InterpreterBuilder(*model, *resolver);
  printf("The End\n");
}
