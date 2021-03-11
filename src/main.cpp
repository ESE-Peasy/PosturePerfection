/**
 * @copyright Copyright (C) 2021  Miklas Riechmann, Ashwin Maliampurakal
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
#include <time.h>

#include <chrono>  // NOLINT [build/c++11]
#include <deque>
#include <iostream>
#include <mutex>
#include <thread>  //NOLINT [build/c++11]
#include <utility>

#include "iir.h"
#include "inference_core.h"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "pipeline.h"
#include "post_processor.h"
#include "posture_estimator.h"
#include "pre_processor.h"

#define MODEL_INPUT_X 224
#define MODEL_INPUT_Y 224

#define NUM_LOOPS 500

bool run_flag = true;

bool displayImage(cv::Mat originalImage,
                  PostProcessing::ProcessedResults processed_results) {
  cv::Scalar blue(255, 0, 0);
  cv::Scalar red(0, 0, 255);
  int imageWidth = originalImage.cols;
  int imageHeight = originalImage.rows;
  int circleRadius = 5;

  for (auto body_part : processed_results.body_parts) {
    if (body_part.status == PostProcessing::Trustworthy) {
      cv::circle(originalImage,
                 cv::Point(static_cast<int>(body_part.x * imageWidth),
                           static_cast<int>(body_part.y * imageHeight)),
                 circleRadius, blue, -1);
    } else {
      cv::circle(originalImage,
                 cv::Point(static_cast<int>(body_part.x * imageWidth),
                           static_cast<int>(body_part.y * imageHeight)),
                 circleRadius, red, -1);
    }
  }

  cv::imshow("Live", originalImage);
  if (cv::waitKey(5) >= 0) {
    return false;
  } else {
    return true;
  }
}

int main(int argc, char const* argv[]) {
  printf("start\n");

  Pipeline::Pipeline p(4);

  int i = 0;
  bool flag = true;
  while (flag) {
    auto next_frame = p.processed_results.pop();

    flag = displayImage(next_frame.raw_image, next_frame.processed_results);

    // i++;
    // if (i >= NUM_LOOPS) break;
  }
}
