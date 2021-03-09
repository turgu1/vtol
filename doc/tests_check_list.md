# VTol Tests Check List

- [ ] Check if receiver is Ok for all channels: (Debug Params > Radiocomms Only; Debug Params > USB Data Output > Radio Data).
- [ ] Check Fail-Safe response: Same setup as before. Check that the fail-safe values are used when transmitter is Off or when transmitter is ON and then turned OFF. Test when the transmitter is ON and is becoming out of range for potential rapid transitions.
- [ ] Checl Fail-Safe response in Forward mode (Debug Params > USB Data Output > Radio Data). As the servos are positionned in some non safe-mode positions and the motors are running, check that they will be going to their safe mode location. Test when the transmitter is ON and is becoming out of range for potential rapid transitions.
- [ ] Check all servos response from joysticks in Forward Mode. (Aileron and Elevator (Different angle left/right for Roll, Same angle left/right for Pitch; Aileron and Elevator on same side at same angle)).
- [ ] Check all servos response from joystick in Hover Mode. (Ailerons at 90 degrees down, Aileron and Elevator don't move for any joystick displacements)


(TBC)