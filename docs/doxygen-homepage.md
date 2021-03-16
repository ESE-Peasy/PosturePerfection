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

\subsection gui Graphical User Interface

The _GUI_ is responsible for user interaction and displaying a live feed of the user's posture. Processing itself is completed within the Pipeline, with the `PostureEstimating::PostureEstimator` responsible for continually passing a `PoseStatus` to the GUI indicating if a good posture is currently being adopted, and any computed changes which would improve the user's posture.

- [MainWindow](@ref GUI::MainWindow)

\subsection notify Notification System

A _[Notification System](@ref Notify)_ has been developed to allow an alert to be sent to a separate device when then system detects a poor posture. In this way, although the PosturePerfection system will be placed at the side of the user to analyse their posture profile, an alert can be sent to their laptop device which they are working on so that they are immediately notified of their poor posture.

- [NotifyClient](@ref Notify::NotifyClient)
- [NotifyServer](@ref Notify::NotifyServer)

\section additional_resources Additional Resources

- [GitHub Repository](http://github.com/ESE-Peasy/PosturePerfection)
- [GitHub Pages](../)
