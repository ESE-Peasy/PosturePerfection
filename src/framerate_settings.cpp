/**
 * @copyright Copyright (C) 2021  Miklas Riechmann
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "pipeline.h"
#define DEFAULT_FRAMERATE_SETTING 1

namespace Pipeline {
FramerateSettings::FramerateSettings(Pipeline* pipeline)
    : framerate_settings(
          {FramerateSetting{
               IIR::SmoothingSettings{std::vector<std::vector<float>>{
                   {{0.20657208, 0.41314417, 0.20657208, 1., -0.36952738,
                     0.19581571}}}},
               1000},
           FramerateSetting{
               IIR::SmoothingSettings{std::vector<std::vector<float>>{
                   {{0.10853539, 0.21707078, 0.10853539, 1., -0.87674856,
                     0.31089012}}}},
               667},
           FramerateSetting{
               IIR::SmoothingSettings{std::vector<std::vector<float>>{
                   {{0.03168934, 0.06337869, 0.03168934, 1., -0.41421356, 0.},
                    {1., 1., 0., 1., -1.0448155, 0.47759225}}}},
               500},
           FramerateSetting{
               IIR::SmoothingSettings{std::vector<std::vector<float>>{
                   {{4.15080543e-04, 8.30161086e-04, 4.15080543e-04,
                     1.00000000e+00, -1.48014304e+00, 5.56155720e-01},
                    {1.00000000e+00, 2.00000000e+00, 1.00000000e+00,
                     1.00000000e+00, -1.70131184e+00, 7.88682638e-01}}}},
               333},
           FramerateSetting{
               IIR::SmoothingSettings{std::vector<std::vector<float>>{
                   {{1.44120224e-04, 2.88240448e-04, 1.44120224e-04,
                     1.00000000e+00, -1.59971967e+00, 6.45176015e-01},
                    {1.00000000e+00, 2.00000000e+00, 1.00000000e+00,
                     1.00000000e+00, -1.78525306e+00, 8.35981369e-01}}}},
               250},
           FramerateSetting{
               IIR::SmoothingSettings{std::vector<std::vector<float>>{
                   {{1.03686193e-05, 2.07372385e-05, 1.03686193e-05,
                     1.00000000e+00, -1.79158770e+00, 8.04092844e-01},
                    {1.00000000e+00, 2.00000000e+00, 1.00000000e+00,
                     1.00000000e+00, -1.90064656e+00, 9.13912934e-01}}}},
               125},
           FramerateSetting{
               IIR::SmoothingSettings{std::vector<std::vector<float>>{
                   {{3.73142259e-06, 7.46284518e-06, 3.73142259e-06,
                     1.00000000e+00, -1.83835927e+00, 8.45909649e-01},
                    {1.00000000e+00, 2.00000000e+00, 1.00000000e+00,
                     1.00000000e+00, -1.92524967e+00, 9.33156924e-01}}}},
               80},
           FramerateSetting{
               IIR::SmoothingSettings{std::vector<std::vector<float>>{
                   {{5.94209980e-07, 1.18841996e-06, 5.94209980e-07,
                     1.00000000e+00, -1.89771159e+00, 9.00749843e-01},
                    {1.00000000e+00, 2.00000000e+00, 1.00000000e+00,
                     1.00000000e+00, -1.95452916e+00, 9.57658381e-01}}}},
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
