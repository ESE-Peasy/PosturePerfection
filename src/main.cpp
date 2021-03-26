/**
 * @copyright Copyright (C) 2021  Miklas Riechmann, Ashwin Maliampurakal, Andrew
 * Ritchie
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

#include <stdio.h>

#include <QApplication>

#include "gui/mainwindow.h"
#include "intermediate_structures.h"
#include "pipeline.h"
#include "post_processor.h"
#include "posture_estimator.h"

#define NUM_LOOPS 500
#define NUM_INF_CORE_THREADS 8

bool run_flag = true;

GUI::MainWindow* main_window_ptr;
Pipeline::Pipeline* pipeline_ptr;

void frame_callback(PostureEstimating::PoseStatus pose_status,
                    cv::Mat input_image) {
  main_window_ptr->updatePose(pose_status);
  main_window_ptr->updateFrame(input_image);
}

int main(int argc, char* argv[]) {
  printf("start\n");
  Pipeline::Pipeline p(NUM_INF_CORE_THREADS, &frame_callback);
  pipeline_ptr = &p;
  QApplication a(argc, argv);
  GUI::MainWindow w(pipeline_ptr);
  QCoreApplication::processEvents();
  w.showMaximized();

  main_window_ptr = &w;

  return a.exec();
}
