# VTOL - Vertical take-off and landing Flight Controller

This is an instance of the Beta 1.2 version of dRehmFlight FC available [here](https://github.com/nickrehm/dRehmFlight)

This is a work in progress and is not ready yet. The flight controller is being developed for an F-35 version of an r/c vertical take-off and landing model aircraft, as described by Nicholas Rehm, using a Teensy 4.1 and IMU 6050.

The following has been added to Nicholas Rehm code:

A C++ Config class (in folder `src/Config`) that supplies the following:

- USB port menus to allow for parameters modification at boot time
- EEPROM parameters save and retrieval

Some of the steps remaining to be done:

- [ ] Code adaptation for targeted model aircraft configuration
- [ ] Telemetry transfer through S-Port
- [ ] Add Angle/Angle2/Rate selection
- [ ] Add USB trace output control
- [ ] Overall verification with selected electronics and hardware
- [ ] Data Logging for post-flight analysis
- [ ] Tests in flight

Other potential changes:

- [ ] Config modification from control transmitter
- [ ] LCD for debugging support
- [ ] Port to a standard quad FC (if fast enough)
- [ ] Other devices integration

Licensing: GPL-3.0

## Config class usage at reset time

At boot time the setup() function of the main program is called to initialize the Teensy for preparation of the main loop function. After the Serial port is initialized, the `Config::setup()` method is called as the next step. This method retrieve the current configuration from the EEPROM and adjust all parameters of the main application. If the EEPROM doesn't contain valid information (a crc32 checksum and a version number are used to insure that), the parameters will be initialized with the default values as defined in the original main application content.

The Config::setup() method will theh wait for 10 seconds for the user to send a carriage return character to the Teensy. If so, it will then show a main menu giving access to all configuration parameters of the main application and several other management functions through many entries and sub-menues. If nothing is received within the 10 seconds, the Config::setup() function will exit and control will be return back to the main application setup function that will resume the preparation of the device.

The interaction with the menues is as follow:

- A menu is presented on the screen with a title and a list of numbered entries.
- A `> ` character is presented on the next line.
- The user enter a number corresponding to the menu entry for which he wants to interact with, followed with a carriage-return. If no number is entered or the escape character is sent, the preceeding menu will be showed to the user.
- If the selected entry correspond to another menu, it will then be presented to the user.
- If the selected entry correspond to a modifiable parameter, the user will be requested to enter a new value for the parameter followed by a carriage-return. If no value is entered or the escape character is sent, the parameter will be left unchanged.
- If the selected entry correspond to an action to be executed, it will be done and a message indicating the result of the action will be showed on screen.

To exit the main menu, it is required to explicitly enter the "Exit" menu entry number. 