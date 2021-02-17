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

Update to include link to Doxygen
{: .label .label-yellow}

## 3. `InferenceCore`

The `InferenceCore` is the heart of the system, and is responsible for performing pose estimation on the input image. Various models are likely to be used for this during the trialling phases, and so the component should be designed to allow the choice of model to be modified relatively easily. The system will then utilise [TensorFlow Lite](https://www.tensorflow.org/lite/) to perform the pose estimation, and structure the output results in a suitable format. The output is expected to be the co-ordinates of the points of interest detected during pose estimation relative to the input image. These co-ordinates are then passed to the `PostProcessor`.

Update to include link to Doxygen
{: .label .label-yellow}

## 4. `PostProcessor`

Since the `PreProcessor` was responsible for resizing the image to suit the model used in the `InferenceCore`, the output co-ordinates must be corrected to the initial image dimensions. This component should also analyse these co-ordinates and discard any points which are not of interest (e.g. feet and knees). The final co-ordinates of interest are then output from this component to the final `Graphical User Interface`, and an alert may be sent from this unit directly to the User's Work Device as outlined in the [System Design](/system/system-design)

Update to include link to Doxygen
{: .label .label-yellow}

## 5. `GraphicalUserInterface`

The `GUI` is responsible for utilising the incoming co-ordinates of interest and displaying a live feed of the user's posture. It is likely other features will also be added to this unit, which will require further design and modification of the system.

Update to include link to Doxygen
{: .label .label-yellow}