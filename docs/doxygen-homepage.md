[TOC]

This is a project completed as part of the 5th Year Engineering course, Real Time Embedded Programming, at The University of Glasgow.

Code documentation can be found here, however more general information regarding the project can be found on our [website](../system/system.html).

\section stages Stages

The system is broken down into various stages to allow us to make use of multithreading such that when one stage is processing one frame,
another can simultaneously operate on an entirely different frame. In this way, a pipeline is created, with ordering only essential during
the PostProcessing stage which applies IIR filtering to smoothen the output.

The stages are as follows:

- Pipeline
  - [PreProcessor](@ref PreProcessing::PreProcessor)
  - [InferenceCore](@ref Inference::InferenceCore)
  - [PostProcessor](@ref PostProcessing::PostProcessor)
  - [PostureEstimator](@ref PostureEstimating::PostureEstimator)
- GUI
  - [MainWindow](@ref GUI::MainWindow)
- Notify
  - [NotifyClient](@ref Notify::NotifyClient)
  - [NotifyServer](@ref Notify::NotifyServer)

\section additional_resources Additional Resources

- [GitHub Repository](http://github.com/ESE-Peasy/PosturePerfection)
- [GitHub Pages](../)
