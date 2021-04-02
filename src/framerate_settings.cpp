#include "pipeline.h"
#define DEFAULT_FRAMERATE_SETTING 1

namespace Pipeline {
FramerateSettings::FramerateSettings(Pipeline* pipeline)
    : framerate_settings(
          {FramerateSetting{
               IIR::SmoothingSettings{std::vector<std::vector<float>>{
                   {{0.03168934, 0.06337869, 0.03168934, 1., -0.41421356, 0.},
                    {1., 1., 0., 1., -1.0448155, 0.47759225}}}},
               1000},
           FramerateSetting{
               IIR::SmoothingSettings{std::vector<std::vector<float>>{
                   {{0.03168934, 0.06337869, 0.03168934, 1., -0.41421356, 0.},
                    {1., 1., 0., 1., -1.0448155, 0.47759225}}}},
               667},
           FramerateSetting{
               IIR::SmoothingSettings{std::vector<std::vector<float>>{
                   {{0.03168934, 0.06337869, 0.03168934, 1., -0.41421356, 0.},
                    {1., 1., 0., 1., -1.0448155, 0.47759225}}}},
               500},
           FramerateSetting{
               IIR::SmoothingSettings{std::vector<std::vector<float>>{
                   {{0.03168934, 0.06337869, 0.03168934, 1., -0.41421356, 0.},
                    {1., 1., 0., 1., -1.0448155, 0.47759225}}}},
               333},
           FramerateSetting{
               IIR::SmoothingSettings{std::vector<std::vector<float>>{
                   {{0.03168934, 0.06337869, 0.03168934, 1., -0.41421356, 0.},
                    {1., 1., 0., 1., -1.0448155, 0.47759225}}}},
               250},
           FramerateSetting{
               IIR::SmoothingSettings{std::vector<std::vector<float>>{
                   {{0.03168934, 0.06337869, 0.03168934, 1., -0.41421356, 0.},
                    {1., 1., 0., 1., -1.0448155, 0.47759225}}}},
               125},
           FramerateSetting{
               IIR::SmoothingSettings{std::vector<std::vector<float>>{
                   {{0.03168934, 0.06337869, 0.03168934, 1., -0.41421356, 0.},
                    {1., 1., 0., 1., -1.0448155, 0.47759225}}}},
               80},
           FramerateSetting{
               IIR::SmoothingSettings{std::vector<std::vector<float>>{
                   {{0.03168934, 0.06337869, 0.03168934, 1., -0.41421356, 0.},
                    {1., 1., 0., 1., -1.0448155, 0.47759225}}}},
               50}}),
      current_setting(DEFAULT_FRAMERATE_SETTING),
      pipeline(pipeline) {}

void FramerateSettings::notify_pipeline(void) {
  pipeline->updated_framerate(framerate_settings.at(current_setting));
}

FramerateSetting FramerateSettings::get_framerate_setting(void) {
  return framerate_settings.at(current_setting);
}

void FramerateSettings::decrease_framerate(void) {
  if (current_setting > 0) {
    current_setting--;
    notify_pipeline();
  }
}

void FramerateSettings::increase_framerate(void) {
  if (current_setting < framerate_settings.size() - 1) {
    current_setting++;
    notify_pipeline();
  }
}

}  // namespace Pipeline
