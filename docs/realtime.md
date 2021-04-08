---
nav_order: 4
title: Realtime Responsiveness Evaluation
---

# Realtime Responsiveness Evaluation

A detailed description of the pipeline used in PosturePerfection is outlined [here](../html/index.html). As a high level overview, the `FrameGenerator` runs in its own thread and makes use of a `CppTimer` to continually read frames from the camera feed into the system at regular intervals. The system then makes use of multithreading as several threads are initialised to perform pre-process the image and run pose estimation on it. Whenever a frame is read by the `FrameGenerator`, it notifies these threads that a new frame is ready for processing. If one of these threads is free, it performs the required pre-processing and pose estimation on that frame before passing it along with the results to the post processing thread. The post processing thread must process frames in order as `IIR` filtering is applied, before running the `PostureEstimator` to determine if the current frame includes the user in a good posture, and if not the changes they need to make to return to a good posture.

## Frame Rate Improvement by using Multi-threading

During initial development, the pipeline did not make use of multi-threading and therefore resulted in a very slow frame rate and high latency in processing a video stream. Therefore a major change was implemented to exploit multi-threading. As mentioned above, the post processing stage must be performed on frames in the correct order due to the use of `IIR` filtering. However, the pose estimation stage can be parallelised to significantly improve performance. Instead of waiting for the stage to process a frame before another can be processed, another waiting thread can process the given frame. Below is an evaluation of how a higher number of threads for this middle stage results in a higher frame rate being able to be used:

## Latency Evaluation
