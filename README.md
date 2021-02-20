# VTOL - Vertical take-off and landing Flight Controller

This is an instance of the Beta 1.2 version of dRehmFlight FC available [here](https://github.com/nickrehm/dRehmFlight)

This is a work in progress and is not ready yet. The flight controller is being transformed for an F-35 version of an r/c vertical take-off and landing model aircraft, as described by Nicholas Rehm, using a Teensy 4.1 and IMU 6050.

The following has been added to Nicholas Rehm code:

A C++ Config class (in folder `src/Config`) that supplies the following:

- EEPROM parameters save and retrieval
- USB port menu to allow for parameters modification at boot time

Some of the steps remaining to be done:

- [ ] Code adaptation for targeted model aircraft configuration (partly done)
- [ ] Telemetry transfer through S-Port
- [ ] Add Angle/Angle2/Rate selection
- [x] Add USB trace output control
- [ ] Testing tools for Motors
- [x] Testing tools for Servos
- [ ] Overall verification with selected electronics and hardware
- [ ] Data Logging for post-flight analysis
- [ ] Tests in flight
- [ ] Assess adding USB menu access in the main loop()

Other potential changes:

- [ ] Config modification from control transmitter
- [ ] LCD for debugging support
- [ ] Port to a standard quad FC (if fast enough)
- [ ] Other devices integration

Licensing: GPL-3.0

## Config class usage at reset time

At boot time the `setup()` function of the main program is called to initialize the Teensy for preparation of the main `loop()` function. After the Serial port is initialized, the `config.setup()` method is called. This method retrieves the current configuration from the EEPROM and adjusts all parameters of the main application. If the EEPROM doesn't contain valid information (a crc32 checksum and a version number are used to ensure that), the parameters are initialized with the default values as defined in the original main application content.

The `config.setup()` method will then wait for 10 seconds for the user to send a carriage return character to the Teensy. If so, it will then show the main menu giving access to all configuration parameters of the main application and several other management functions. If nothing is received within 10 seconds, the `config.setup()` function will exit and control will be returned to the main application setup function that will resume the preparation of the device.

## The menu system

As stated, the Config class supplies a facility to modify any parameters of the main application through a series of menus. Menu entries are of the following kinds:

- Parameter: The current parameter name is shown in squared brackets, the value is in parentheses and can be modified if selected.
- Menu: If selected, the entry will show a sub-level menu.
- Task: A simple task will be launched if selected.
- Exit: Return to the main application.

The interaction with the menus is as follow:

- A menu is presented on the screen with a title and a list of numbered entries.
- A `> ` character is presented in the next line.
- The user enters a number corresponding to the menu entry for which he wants to interact with, followed by a carriage-return. If no number is entered or the escape character is sent, the preceding menu will be shown to the user. If the current menu is the Main Menu, it will be refreshed.
- If the selected entry corresponds to another menu, it will then be presented to the user.
- If the selected entry corresponds to a modifiable parameter, the user will be requested to enter a new value for the parameter followed by a carriage-return. If no value is entered or the escape character is sent, the parameter will be left unchanged. For choice selectable parameters, a list of choices is presented and the corresponding choice number must be entered.
- If the selected entry corresponds to an action to be executed, it will be done and a message indicating the result of the action will be shown on screen.

To exit the main menu, it is required to explicitly enter the **Exit** menu entry number. 

Any change made to a parameter is immediately modified in the running application. It is NOT updated in the saved parameters in EEPROM, this to allow for testing when the end state may not be stable or when there are other parameters to update. The user must explicitly request the main menu entry **Save Params to EEPROM** to have all parameters be saved in EEPROM.

The debugging menu is used to set debug support variables in the application. These variables are *not* saved in EEPROM and, if changed, will retrieve their default value at boot time.

Once control is returned to the main application, the config class is not allowed to regain control unless the Teensy is reset. This is to limit interfering with the main `loop()` function once it has been started. *The impact: parameter changes that have not been saved before leaving control to the main application will not be retained.* This behavior will be revisited once the impact of adding access to the parameters menu from the main `loop()` function is tested and conclusive.

## Modifications done to the Main application

The following changes have been made so far to the main source code `src/dRehmFlight_Tensy_BETA_1.2.ino`. This will be updated as changes are being done.

- New User parameter: `USB_output`, `receiver_only` for debugging purposes. Their value is modifiable through the menu system.
- In the `setup()` function, call to the `config.setup()` method. Before the `setup()` function, an `#include` statement to get the Config class available.
- in the `loop()` function, a *switch* statement to select which debugging output to send to the USB port.
- Only 3 motors are being used, Motors 4..6 have been commented out. The 3 used motor are associated with Pins 0, 1, 2
- Only 3 servos are being used, Servos 4..7 have been commented out. The 3 used servos are associated with Pins 3, 4, 5
- Channel numbers are separated from their PWM values: name of channel_x_pwm variables changed to relate them to their functions: channel_1_pwm is now throttle_pwm, channel_2_pwm is now aileron_pwm, and also for elevator, rudder, throttle_cut, and aux1. The same is done for the channel_x_pwm_prev variables. In the user-specific parameters, new parameters are used to associate channels with their functions. Current default values reflect the original code. This is to simplify the user preference changes for channel associations.
- Code cleanup to get rid of compilation warning messages.
- FailedSafe() modified to take care of the sbusFailSave value when SBUS is being used.
  
## Hardware configuration

- Transmitter: FrSky Taranis X9D
- Receiver: R-XSR (SBus)
- ESC: (Oneshot125)
- 3 Motors 2206
- 3 PWM Servos