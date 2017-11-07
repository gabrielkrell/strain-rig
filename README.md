# strain-rig

Working notes on the current code and project setup:

Components:
 - Arduino Nano - controls other peripherals, manages UI. Uses [Arduino-PID-Library](https://github.com/br3ttb/Arduino-PID-Library)
 - HX711 - 24-bit ADC used to read force sensor. Uses [Queuetue HX711 library](https://github.com/queuetue/Q2-HX711-Arduino-Library/).
 - Motor driver board with L298N: drives stepper motor.
 
Interaction over serial:
 - Input `m123456.789`: change setpoint to input number (in mm?).
 - Input `z`: Zero the force sensor.
