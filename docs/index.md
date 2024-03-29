---
nav_order: 1
title: Welcome!
---

<div align="center"> 
  <img src="images/logo.gif" alt="Logo for PosturePerfection" width="300">
  <h1 class="fw-300">Welcome to Posture Perfection!</h1>
  <hr/>
</div>

<div align="center" class="fs-6 fw-300"> 
  We are a group of 5th Year Electronics & Software Engineering students at the University of Glasgow, completing a project which provides you with real-time notifications when we detect you have adopted a poor seated posture which could be detrimental to your future health.
</div>

With the rapidly increased number of individuals around the world being forced to "work from home", we are aware that many of you have far from ideal setups for your working day, with a lack of office-made chairs and desks as you spend the majority of your day at your work device. Naturally this leads to significant discomfort as you might find yourself gradually slouching further and further into your desk over the course of the day. Not only does this lead to immediate pain in the form of a strained neck or a sore back, but further down the line this can lead to even more significant health impacts. The main difficulty with this is that you may not even notice just how poor your posture is when you are seated for extensive periods of time!

<div align="center">
  <img src="images/animation1.gif" alt="Animation of PosturePerfection" width="600">
</div>

Therefore we wanted to create a system which notifies you when it detects a poor posture. This should encourage you to try and resolve the problem, but we are also hoping to incorporate a system which provides you with real-time visual feedback to guide you back to a "good" posture. We understand that different people are going to have different setups at their home and that the machine learning model we use for pose estimation on the Raspberry Pi will never perform quite as well as on higher-end machines. Therefore we all you to tune your usage of the system by setting your own "ideal" posture when you feel you are seated well. This is then what we use as a reference to guide you back to when we detect you have started slouching. Additionally, we have included a _Settings_ page where you can try out different confidence values for the model we are using, modify the _Slouch Sensitivity_ and also the frame rate of our system so you can optimise your experience with PosturePerfection!

## Demonstration

Check out a demonstration of PosturePerfection running on a Raspberry Pi 4B below:

<div align="center">
  <video width="600" controls>
    <source src="videos/PosturePerfectionv1.0.0.mp4" type="video/mp4">
  </video>
</div>

## Support or Contact

Please reach out to us [here](about-us) if you have any questions.
