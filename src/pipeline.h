/**
 * @file pipeline.h
 * @brief Main pipeline for the PosturePerfection system
 *
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

#ifndef SRC_PIPELINE_H_
#define SRC_PIPELINE_H_

#include <deque>
#include <mutex>
#include <thread>  //NOLINT [build/c++11]
#include <vector>

#include "iir.h"
#include "inference_core.h"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "post_processor.h"
#include "pre_processor.h"

namespace Pipeline {

template <typename T>
class Buffer {
 private:
  std::mutex lock_in;
  std::mutex lock_out;
  std::deque<T> queue;
  bool* running;

 public:
  uint8_t id = -1;
  Buffer(bool* running_ptr) { this->running = running_ptr; }
  void push(T frame) {
    while (*running) {
      this->lock_in.lock();
      if ((uint8_t)(this->id + 1) == frame.id) {
        this->queue.push_back(frame);
        this->id++;
        this->lock_in.unlock();
        break;
      }
      this->lock_in.unlock();
    }
  }
  T pop() {
    T front;
    while (*running) {
      this->lock_out.lock();
      if (!this->queue.empty()) {
        front = this->queue.front();
        this->queue.pop_front();
        this->lock_out.unlock();
        break;
      }
      this->lock_out.unlock();
    }
    return front;
  }
};

struct PreprocessedFrame {
  uint8_t id;
  cv::Mat raw_image;
  PreProcessing::PreProcessedImage preprocessed_image;
};

struct CoreResults {
  uint8_t id;
  cv::Mat raw_image;
  Inference::InferenceResults image_results;
};

struct ProcessedResults {
  uint8_t id;
  cv::Mat raw_image;
  PostProcessing::ProcessedResults processed_results;
};

class Pipeline {
 private:
  std::vector<std::thread> threads;
  bool running;

  PreProcessing::PreProcessor preprocessor;
  PostProcessing::PostProcessor post_processor;

  Buffer<PreprocessedFrame> preprocessed_frames;
  Buffer<CoreResults> core_results;

  void input_thread_body(void);
  void core_thread_body(Inference::InferenceCore);
  void post_processing_thread_body(void);

 public:
  Buffer<ProcessedResults> processed_results;
  Pipeline(uint8_t num_inference_core_threads);
  ~Pipeline();
};
}  // namespace Pipeline
#endif
