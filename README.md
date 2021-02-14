# VTOL - Vertical take-off and landing Flight Controller

This is an instance of the Beta 1.2 version of dRehmFlight FC avalable [here](https://github.com/nickrehm/dRehmFlight)

This is work in progress and is not ready yet. The flight controller is being developed for an F-35 version of an r/c vertical take-off and landing aircraft, as described by Nicholas Rehm, using a Teensy 4.1 and IMU 6050.

The following has been added to Nicholas Rehm code:

A C++ Config class (in folder `src/Config`) that supplies the following:

- USB port menus to allow for parameters modification at boot time
- EEPROM parameters save and retrieval

Licensing: GPL-3.0