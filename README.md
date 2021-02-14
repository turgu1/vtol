# VTOL - Vertical take-off and landing Flight Controller

This is an instance of the Beta 1.2 version of dRehmFlight FC avalable [here](https://github.com/nickrehm/dRehmFlight)

This is work in progress and is not ready yet. The flight controller is being developed for an F-35 version of an r/c vertical take-off and landing aircraft, as described by Nicholas Rehm, using an Teensy 4.1.

The following has been added to Nicholas Rehm code:

A C++ Config class (in folder `src/Config`) that supplies the following:

- USB port menus to allow for parameters modification at boot time
- Parameters are saved in EEPROM and retrieved at boot time

Licensing: GPL-3.0