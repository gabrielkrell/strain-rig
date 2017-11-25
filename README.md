# strain-rig

## New version notes

Boards:
 - Arduino MEGA 2560 - controls stepper, reads force input.
 - HX711 - 24-bit ADC used to read force sensor. Uses [Queuetue HX711 library](https://github.com/queuetue/Q2-HX711-Arduino-Library/).
 - [EasyDriver](http://www.schmalzhaus.com/EasyDriver/) stepper driver. Max frequency 500kHz (2 us/cycle, far faster than motor can do).

Parts: 
 - [Stepper Motor](https://www.pololu.com/product/1200): 200 Steps/Rev, 1.2A/Phase. 9.09kHz (110 us/cycle) seems to be good max frequency.
 - [Linear screw](https://www.smw3d.com/sfu-1204-ball-screw-and-flanged-nut/): 4mm lead. See [ball screw guide](http://www.anaheimautomation.com/manuals/forms/ball-screw-guide.php#sthash.fK6LyzyL.WL0C372z.dpbs).

Interaction over serial:
 - Input `m123456.789`: change setpoint to input number in cm.
 - Input `z`: Zero the force sensor.

## To compile:
1. Install the [Queuetue HX711 library](https://github.com/queuetue/Q2-HX711-Arduino-Library/) (Library Manager > Queuetue HX711 library).

## Old version notes

Components:
 - Arduino Nano - controls other peripherals, manages UI. Uses [Arduino-PID-Library](https://github.com/br3ttb/Arduino-PID-Library)
 - HX711 - 24-bit ADC used to read force sensor. Uses [Queuetue HX711 library](https://github.com/queuetue/Q2-HX711-Arduino-Library/).
 - Motor driver board with L298N: drives stepper motor.
 
Interaction over serial:
 - Input `m123456.789`: change setpoint to input number (in mm?).
 - Input `z`: Zero the force sensor.

Extra compilation steps: install the [Arduino-PID-Library](https://github.com/br3ttb/Arduino-PID-Library) (Library Manager > PID by Brett Beauregard)
