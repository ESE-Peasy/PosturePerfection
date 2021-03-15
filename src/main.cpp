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

#include "intermediate_structures.h"
#include "pipeline.h"
#include "post_processor.h"
#include "posture_estimator.h"

#define NUM_LOOPS 500
#define NUM_INF_CORE_THREADS 4

bool run_flag = true;

void new_frame_callback(PostureEstimating::PoseStatus pose_status,
                        cv::Mat input_image) {
  printf("%s   ", (pose_status.bool_good_posture) ? "good" : "bad ");
  for (auto current_pose_joint : pose_status.current_pose) {
    printf("%.1f %.1f  ", current_pose_joint);
  }
  printf("\r");
}

int main(int argc, char const* argv[]) {
  printf("start\n");

  pritnf("      ---------------------- Current Pose -------------------\n");
  printf("Pose | Head     Neck     Shoulder Hip      Knee     Foot     |\n");

  Pipeline::Pipeline p(NUM_INF_CORE_THREADS, &new_frame_callback);

  while (getchar() != 'q') {
  }
}
