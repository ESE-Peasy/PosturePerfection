---
nav_order: 1
title: System Design
parent: System Overview
---

# System Diagram

This section outlines an overview of the PosturePerfection system, with more detailed information available in our [Doxygen documentation](../html/index.html):
{: .fs-6 .fw-300 }

<div align = "center">
<img src="../images/system-diagram.pdf" alt="Image of System Diagram">
</div>

## System Description

### Camera

The camera provides a live video stream of the user to the Raspberry Pi. The team has chosen the Raspberry Pi Camera V2 for ease of use, but any camera suitable for use with the Pi should be a valid option. The camera will be placed side on, so that the side profile of the user pose can be viewed.

### Raspberry Pi

The Raspberry Pi will handle all processing of the incoming video feed. The team has currently chosen to use the 4GB and 8GB models of the Raspberry Pi Model 4B, but further research may be done on older models if times permits. The Raspberry Pi will make use of a trained neueral network model to perform pose estimation on the incoming video stream. Real-time considerations will have to be made so that pose correction feedback can be provided to the user.

### Screen

This will provide a live update of the user's current pose, using the data generated from the Raspberry Pi. A notification will be displayed whenever the user has adopted a poor posture for a given time period. The user will then be provided with real time feedback via this screen, to indicate the changes they must make to improve their posture. As they attempt to change their posture, these changes will be processed by the Raspberry Pi in real time until the user adopts a healthy posture.

### User's Work Device

Alongside the alert which is displayed on the dedicated screen for the device, configuration will allow further notifications to be sent to the user's main computer device e.g Desktop PC, Laptop etc. This may be in the form of an email or a desktop client running on the device.

# System Interactions

Below is an indication of how the system detects the user's current posture and if it is a healthy posture or not. More details about specific components can be in our [Sequence Diagram](./sequence-diagram.html) and [Class Diagram](./class-diagram.html) pages.

<div align = "center">
<img src="../images/software-flow.pdf" alt="Image of System Flow Diagram">
</div>
