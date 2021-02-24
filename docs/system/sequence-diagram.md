---
nav_order: 2
title: Sequence Diagram
parent: System Overview
---

# Sequence Diagram

To more easily understand the exact process of analysing the video input and performing pose estimation on it, the following sequence diagram was created:

<div align = "center">
<img src="../images/sequence-diagram.png" alt="Sequence Diagram for PosturePerfection">
</div>

The system is separated into five main components:

## 1. `PiCamera`

A module will need to be written which is solely responsible for utilising the incoming video stream from the camera being used. Although this component is not responsible for any processing, it must allow a variable frame rate to be adjusted during testing phases, as it is likely this will need to be altered to account for the processing capabilities of the Raspberry Pi. This component is expected to output single frames to the `PreProcessing` unit.

Update to include link to Doxygen
{: .label .label-yellow}

## 2. `PreProcessor`

The `PreProcessor` is responsible for performing some initial processing on each frame of the video stream as it comes in. At the very least, the image will need to be resized and normalised to suit the model being used in the `InferenceCore`, however it is also possible that some further filtering can be completed to improve the performance of the pose estimation model. This could be filtering the image to discard sections which are not of interest, such as the background of the picture.

[PreProcessor Documentation](../html/classPreProcessing_1_1PreProcessor.html){: .btn .btn-green}

## 3. `InferenceCore`

The `InferenceCore` is the heart of the system, and is responsible for performing pose estimation on the input image. Various models are likely to be used for this during the trialling phases, and so the component should be designed to allow the choice of model to be modified relatively easily. The system will then utilise [TensorFlow Lite](https://www.tensorflow.org/lite/) to perform the pose estimation, and structure the output results in a suitable format. The output is expected to be the co-ordinates and confidence value of the points of interest detected during pose estimation relative to the input image. These co-ordinates and confidence values are then passed to the `PostProcessor`.

[InferenceCore Documentation](../html/classInferenceCore.html){: .btn .btn-green}

## 4. `PostProcessor`

The input co-ordinates to the `PostProcessor` from the `InferenceCore` will be relative, and therefore can be used directly with the initial input image from the `PiCamera`. This component is then responsible for analyse the confidence level of the various points detected and performing some thresholding to discard values which are not deemed to be confident enough, and other points which may not be of interest. The final co-ordinates of interest are then output from this component to the `PoseEstimator`.

[PostProcessor Documentation](../html/classPostProcessing_1_1PostProcessor.html){: .btn .btn-green}

## 5. `PoseEstimator`

The `PoseEstimator` is then responsible for using the co-ordinates received to determine if the user's posture is healthy or not. If a poor posture is detected, this should be immediately brought to the user's attention on the `Graphical User Interface` and an additional alert may be sent from this unit directly to the user's work device if configured. The final output of this should be easily usable by the `Graphical User Interface` to allow a visual representation of the real-time processing to be displayed.

[PostureEstimator Documentation](../html/classPostureEstimating_1_1PostureEstimator.html){: .btn .btn-green}

## 5. `GraphicalUserInterface`

The `GUI` is responsible for utilising the incoming data from the `PoseEstimator` and displaying a live feed of the user's posture. It is likely other features will also be added to this unit, which will require further design and modification of the system.

Update to include link to Doxygen
{: .label .label-yellow}
