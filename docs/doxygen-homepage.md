[TOC]

This is a project completed as part of the 5th Year Engineering course, Real Time Embedded Programming, at The University of Glasgow.

Code documentation can be found here, however more general information regarding the project can be found on our [website](../).

\section breakdown Project Breakdown

The project is separated into three main sections, each with a unique function.

> 1. The Pipeline is responsible for performing pose estimation to detect the user's posture and identify when it has worsened as well as the adjustments required to guide the user into a better posture.\n
> 2. The GUI is responsible for displaying these results and posture adjustment suggestions to the user, as well as allowing various configuration options to be set.\n
> 3. The [Notification System](@ref Notify) allows an alert to be sent to a separate device which the user is using.

\subsection pipeline Pipeline

The system is broken down into various stages to allow us to make use of multithreading such that when one stage is processing one frame,
another can simultaneously operate on an entirely different frame. In this way, a pipeline is created, with ordering only essential during
the PostProcessing stage which applies IIR filtering to smoothen the output.

The _Pipeline_ at the core of the system is configurable to run with a various number of threads. Initial testing indicated optimal performance at 8 threads, and so this is currently in use within the pipeline. The pipeline consists of the following key stages:

- [PreProcessor](@ref PreProcessing::PreProcessor)
- [InferenceCore](@ref Inference::InferenceCore)
- [PostProcessor](@ref PostProcessing::PostProcessor)
- [PostureEstimator](@ref PostureEstimating::PostureEstimator)

The [FrameGenerator](@ref Pipeline::FrameGenerator) runs in a single thread, ensuring that frames are continually captured from the OpenCV camera input at regular intervals defined by the user set frame rate. A timer is used to capture the frames used at regular intervals, making use of the `CppTimer` wrapper developed [here](https://github.com/berndporr/cppTimer).

The [PreProcessor](@ref PreProcessing::PreProcessor) and [InferenceCore](@ref Inference::InferenceCore) run in a single thread, of which 8 are run simulataneously by default. When one of these threads is ready, the latest frame captured from the [FrameGenerator](@ref Pipeline::FrameGenerator) is passed through these two stages. By making use of this threading approach, a more performant system is achieved than if threading was not used.

The [PostProcessor](@ref PostProcessing::PostProcessor) and [PostureEstimator](@ref PostureEstimating::PostureEstimator) run in another thread. Ordering is essential for IIR filtering, and therefore throughout these different stages a unique ID is incremented and passed with each frame. Upon reaching the [PostProcessor](@ref PostProcessing::PostProcessor) stage, IIR filtering is applied using the frames in order, before running posture estimation. Finally, using the results of posture estimation, a rendering of the user's posture is overlayed on the image itself. Note that for the scope of this project, a decision was made to use only the upper body joints of the user's posture as these are the most likely to be visible when the user places their camera side on and are the joints which contribute most significantly to a user's posture. However, extension to use the lower body joints is straightforward should this be desired in the future.

- If an "Ideal Posture" has not been set, then the posture is shown using blue lines, and the `GUI` indicates to the user that they have not yet initialised an "Ideal Posture".
- If two connected joints of the user's current posture remain within a user configurable threshold of the set "Ideal Posture" then this segment is shown as a green line. If **all** joints are within this threshold then all lines will be green and the user will be notified that they have achieved "PosturePerfection".
- If two connected joints of the user's current posture are outwith this threshold, then a red line is drawn for this segment, and a blue arrow indicates the direction this section of the user's body must move in order to return to their "Ideal Posture".
- Finally, if pose estimation fails on a given frame and no joints are detected with a confidence higher than a user configurable threshold, then no arrows are overlayed on the image and the user is notified that they their current posture is "Undefined". This is likely to occur when the user is no longer visible in the image captured by the video feed.

The image and the resulting [PoseStatus](@ref PostureEstimating::PoseStatus) are then passed to the Graphical User Interface.

\subsection gui Graphical User Interface

The _GUI_ is responsible for user interaction and displaying a live feed of the user's posture. Processing itself is completed within the Pipeline, with the `PostureEstimating::PostureEstimator` responsible for continually passing a `PoseStatus` to the GUI indicating if a good posture is currently being adopted, and any computed changes which would improve the user's posture.

- [MainWindow](@ref GUI::MainWindow)

\subsection notify Notification System

A _[Notification System](@ref Notify)_ has been developed to allow an alert to be sent to a separate device when then system detects a poor posture. In this way, although the PosturePerfection system will be placed at the side of the user to analyse their posture profile, an alert can be sent to their laptop device which they are working on so that they are immediately notified of their poor posture.

- [NotifyBroadcast](@ref Notify::NotifyBroadcast)
- [NotifyReceiver](@ref Notify::NotifyReceiver)

\section additional_resources Additional Resources

- [GitHub Repository](http://github.com/ESE-Peasy/PosturePerfection)
- [GitHub Pages](../)
- [Hackaday.io](https://hackaday.io/project/178429-postureperfection)
