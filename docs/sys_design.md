---
nav_order: 4
title: System Outline 
---

# System Diagram
<div align = "center">
<img src="images/system_diagram.png" alt="Image of System Diagram">
</div>
# System Description
## Camera
The camera provides visual information of the user to the Raspberry Pi. The team has chosen the Raspberry Pi camera for ease of use, but any camera suitable for use with the Pi should be a valid option. The camera will be placed side on, so that the side profile of the users pose can be seen. 

## Raspberry Pi
The Raspberry Pi will handle all processing of the incoming images. The team has currently chosen to use the Pi 4 4GB and 8GB models, but further research may be done on older models if times permits. The Raspberry Pi will either use a trained neueral network model or computer vision depending on limitations discovered during the project. Real Time considerations will have to be made, so the user can have live feedback on their pose.

## Screen
This will provide a live update of the user's current pose, using the data generate from the Raspberry Pi. A notification will be displayed whenever the user's pose has been bad for a given time period. They will provided with onscreen real time feedback to show what they have to correct to improve their pose. As they attempt to change their posture, this changes will be processed by the Raspberry Pi in real time and new suggestions will be given, if the user is still not in a correct pose.

## User's Work Device
Alonside the screen notification, furhter notifications will be sent to the user's main computer device e.g Desktop PC, Laptop etc. This may be in the form of an email or a desktop client also running on the device. 

# Software Systems Interactions
<div align = "center">
<img src="images/software_flow.png" alt="Image of System Flow Diagram">
</div>

# Inital UML Diagrams
As the software is still be researched, this is a representation of what roughly will be expected of the program

<div align = "center">
<img src="images/UML_diagram.png" alt="Image of System UML Diagram">
</div>
## Pose
This will be an object representation of the user's pose. There is a boolean denoting if the pose is good or bad, as per specific pose requirements e.g neck straight, shoulder over hips etc. There is also a list, pose_co_ords, which will contains the location of important pose points. This may change depending on the pose identifying method. There is another list of co-ordinates that will represent the user's ideal pose.

## Pose_Estimater
This represents the neural network/computer vision handler that will be implemented. It will contain a pose object and a boolean for enabling/disabling. This class will handle processing images from the camera to identify the current user's pose, as well as calculating the ideal user's pose from the pre-calibrated pose. There is the option to disable/enable the pose_estimation, giving the user a break etc.  

## Display
Further buttons detailed in the wire frames may need to be added as functions, depending how the project progresses. Similarly depending on the UI libraries etc used, the UI objects will be defined. The display class will handle updating the screen as well as starting calibration. There is then also the options for updating the user when their posture has become bad.  
