# VTOL - Vertical take-off and landing Flight Controller

This is an instance of the Beta 1.2 version of dRehmFlight FC available [here](https://github.com/nickrehm/dRehmFlight)

This is a work in progress and is not ready yet. The flight controller is being developed for an F-35 version of an r/c vertical take-off and landing model aircraft, as described by Nicholas Rehm, using a Teensy 4.1 and IMU 6050.

The following has been added to Nicholas Rehm code:

A C++ Config class (in folder `src/Config`) that supplies the following:

- USB port menus to allow for parameters modification at boot time
- EEPROM parameters save and retrieval

Some of the steps remaining to be done:

[ ] Code adaptation for targeted model aircraft configuration
[ ] Telemetry transfer through S-Port
[ ] USB trace output control through a developed menu system
[ ] Overall verification with selected electronics and hardware
[ ] Data Logging for post-flight analysis
[ ] Tests in flight

Other potential changes:

[ ] Config modification from control transmitter
[ ] LCD for debugging support
[ ] Port to a standard quad FC (if fast enough)
[ ] Other devices integration

Licensing: GPL-3.0