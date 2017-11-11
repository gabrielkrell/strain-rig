#define DEBUG

#include <Q2HX711.h>
Q2HX711 hx711(A2, A3);
float TICKS_PER_GRAM = 720000 / 50; // experimentally determined? 720k = 50g
long int zeroValue;

static int downPin = 8;
static int upPin = 5;
static int encA = 3;
static int encB = 2;
static int loadA = A2;
static int loadB = A3;

//PWM values are 45-255
//23,945.76 steps per 1 rev
//40 rev / 1 inch
//957,830.4 steps / inch
// 25.4 mm / inch
// 37,709.86 steps per mm
//37.71 steps per um

static int stepConst = 3771; //divide by 100 before using

volatile long encoderValue = 0;

double serialNumBuffer = 0;
char serialCharBuffer = 'X';

unsigned long timeStarted = 0;

#include <PID_v1.h>
double input, output, output2, finalOutput;
double setpoint = 0;
PID pid(&input, &output, &setpoint, 0.25, 0, 0, DIRECT);
PID pid2(&input, &output2, &setpoint, 0.25, 0, 0, REVERSE);

void setup() {
  pinMode(upPin, OUTPUT);
  pinMode(downPin, OUTPUT);
  digitalWrite(upPin, LOW);
  digitalWrite(downPin, LOW);

  pinMode(encA, INPUT_PULLUP);
  pinMode(encB, INPUT_PULLUP);
  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);

  Serial.begin(9600);

  delay(250);

  zeroScale();
  encoderValue = 0;

  input = (double)encoderValue;
  pid.SetMode(AUTOMATIC);
  pid.SetSampleTime(10); //pid recompute interval in ms
  pid2.SetMode(AUTOMATIC);
  pid2.SetSampleTime(10);

}

void loop() {
  // get variables all at once to avoid waiting for serial buffer to clear
  unsigned long time_ = micros() - timeStarted;
  double pos = encoderValue/37.71;
  double force = readForce(zeroValue);
  Serial.print("Time (micros): ");
  Serial.print(time_);
  Serial.print(" Current Position (um): ");
  Serial.print(pos);
  Serial.print(" Force (g): ");
  Serial.print(force);

  #ifdef DEBUG
  Serial.print(" ");
  Serial.print(setpoint/37.71);
  Serial.print(" ");
  Serial.print(output);
  Serial.print(" ");
  Serial.print(output2);
  #endif
  Serial.println();

  interpretCommand();

  input = (double)encoderValue;
  pid.Compute();
  pid2.Compute();

  analogWrite(upPin, output);
  analogWrite(downPin, output2);

}

void moveMotor() {
  finalOutput = output - output2;

  if (finalOutput > 0) {
    analogWrite(upPin, finalOutput);
    digitalWrite(downPin, LOW);
  }
  analogWrite(downPin, output2);
  digitalWrite(upPin, LOW);
}

float readForce(long int zeroValue) {
  long int sensorVal = hx711.read() - zeroValue;
  return (float)(sensorVal / TICKS_PER_GRAM);
}

void zeroScale() {
  zeroValue = (hx711.read() + hx711.read() + hx711.read())/3;
  Serial.println("zeroing");
}

void updateEncoder() {
  volatile static int lastEncoded = 0;
  // Encoder states are Gray code: 00-01-11-10. "transitions" tells
  // which way we're moving. For example,
  // transitions[0b00][0b01] == 1 (00 to 01 is "forward") and
  // transitions[0b01][0b00] == -1 (01 to 00 is other direction).
  // Illegal changes are 0s (no change). For more, see
  // See http://bildr.org/2012/08/rotary-encoder-arduino/
  static const int transitions[4][4] = {
    { 0,  1, -1,  0},
    {-1,  0,  0,  1},
    { 1,  0,  0, -1},
    { 0, -1,  1,  0}};

  byte MSB = digitalRead(encA); // most significant bit
  byte LSB = digitalRead(encB); // least significant bit

  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  encoderValue -= transitions[lastEncoded][encoded];
  lastEncoded = encoded; //store this value for next time
}

void interpretCommand() {

  if (Serial.available() > 0) {
    serialCharBuffer = Serial.read();
    serialNumBuffer = Serial.parseFloat();

    if (serialCharBuffer == 'm') {
      if (!timeStarted) {
        // on first move, update start time
        timeStarted = micros();
      }
      setpoint = (serialNumBuffer * stepConst ) / 100;
      serialCharBuffer = 'X';
//      Serial.println("Moved ");
//      if (serialNumBuffer < 0) {
//        Serial.print("down ");
//      }
//      else {
//        Serial.print("up ");
//      }
//      Serial.print(serialNumBuffer); Serial.print("mm"); Serial.println();
    }
    else if (serialCharBuffer == 'z') {
      zeroScale();
    }
  }
}


