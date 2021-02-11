#include <tflite.h>

tflite::StderrReporter error_reporter;

auto model = tflite::FlatBufferModel::BuildFromFile(
    "../models/EfficientPoseRT_LITE.tflite", &error_reporter);

tflite::MyOpResolver resolver; 

tflite::InterpreterBuilder(model, resolver);
