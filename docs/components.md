---
nav_order: 3
title: Hardware Setup
---

# Hardware Setup
If you would like to setup PosturePerfection using your own Raspberry Pi follow the steps below!
{: .fs-6 .fw-300 }
## You will need:
- Raspberry Pi
  - We use a Raspberry Pi 4B for testing, but other versions may work.
- Raspberry Pi Camera Module
- Screen/monitor
- SD Card
  - Minimum recommended storage size is 8GB
  - Note that Raspberry Pi model 3 or higher takes a micro SD card
- USB type C power supply
- Optionally, a case for the Raspberry Pi and camera

Assuming a Raspberry Pi, power supply, and screen/monitor are already available, we estimate the remaining components will cost between £20-£30.

## Assembly
This section covers the hardware assembly. For software setup checkout the [software setup guide](software-install).

1. Attach the camera module to the CSI port. This is the slot closest to the Ethernet (or furthest from the SD card). Lift up the plastic toggle and slide in the camera ribbon cable. Ensure the contacts on the cable face in the correct direction. Push down the toggle softly to fix the ribbon cable in place.
1. Attach the screen or monitor. You will either attach this via a HDMI port (on the side) or the other ribbon cable port.
1. Insert the flashed SD card (see [the official Raspberry Pi guide](https://projects.raspberrypi.org/en/projects/raspberry-pi-setting-up) for instructions on flashing).
1. Enable the camera functionality on your Raspberry Pi by running ```sudo raspi-config``` in the terminal, then navigating to *Interfacing Options*, select *Camera* and finally select *Enable*. You will need to reboot the Pi to activate the change.
1. Attach the power supply and you're good to go!
