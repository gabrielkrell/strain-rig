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
 - Input `m12.3456789`: change setpoint to input number in cm.
 - Input `z`: Zero the force sensor.

## To compile:
1. Install the [Queuetue HX711 library](https://github.com/queuetue/Q2-HX711-Arduino-Library/) (Library Manager > Queuetue HX711 library).

## Linux usage instructions

The actual UI isn't done yet, but you can still use it with the command line. To use on Linux, you can install the Arduino IDE (probably), or just use built-in terminal stuff:

1. Plug in the Arduino, and figure out which port it's on:
   `$ dmesg | grep tty`
  
   Example output:
   ```
   gabe@dormserver:~$ dmesg | grep tty
   [    0.000000] console [tty0] enabled
   [    1.341091] tty tty2: hash matches
   [1249326.491407] cdc_acm 3-2:1.0: ttyACM0: USB ACM device
   ```
   (Here, the Arduino will be `/dev/ttyACM0`.)
2. Make sure your user is in the `dialout` group, which lets you use serial devices. For example:	

   `sudo usermod -a -G dialout gabe`

   You may need to log out and log in again for this to take effect.
3. Now, set the tty parameters with `stty` (replacing `/dev/ttyACM0` with whatever your device is called):

   ```
   sudo stty -F /dev/ttyACM0 cs8 115200 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts -hupcl
   ```
4. Now, you can write to Arduino like you would any file or device. For example, to move to the 3cm position:

   `echo "m3" > /dev/ttyACM0`
5. One way to view the Arduino's output is

   `cat /dev/ttyACM0`

## Old version notes

Components:
 - Arduino Nano - controls other peripherals, manages UI. Uses [Arduino-PID-Library](https://github.com/br3ttb/Arduino-PID-Library)
 - HX711 - 24-bit ADC used to read force sensor. Uses [Queuetue HX711 library](https://github.com/queuetue/Q2-HX711-Arduino-Library/).
 - Motor driver board with L298N: drives stepper motor.
 
Interaction over serial:
 - Input `m123456.789`: change setpoint to input number (in mm?).
 - Input `z`: Zero the force sensor.

Extra compilation steps: install the [Arduino-PID-Library](https://github.com/br3ttb/Arduino-PID-Library) (Library Manager > PID by Brett Beauregard)
