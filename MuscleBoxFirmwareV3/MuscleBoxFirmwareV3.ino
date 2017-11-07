#include <Q2HX711.h>
Q2HX711 hx711(A2, A3);
long int zeroAvgValue[3];
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


volatile int lastEncoded = 0;
volatile long encoderValue = 0;
long lastencoderValue = 0;
int lastMSB = 0;
int lastLSB = 0;

int motorSpeed = 0;

double serialNumBuffer = 0;
char serialCharBuffer = 'X';

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
  Serial.println();

  Serial.print("Current Position (um): ");
  Serial.print(encoderValue/37.71);
  Serial.print(" Force (g): ");
  Serial.print(readForce(zeroValue));
//  Serial.print(" ");
//  Serial.print(setpoint/37.71);
  Serial.print(" ");
  Serial.print(output);
  Serial.print(" ");
  Serial.print(output2);

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
  //720000 = 50.00g
  long int sensorVal;
  float returnVal;

  float scaleConstant = 720000 / 50;

  sensorVal = hx711.read() - zeroValue;
  returnVal = sensorVal / scaleConstant;

  return returnVal;
}

void zeroScale() {
  zeroAvgValue[1] = hx711.read();
  zeroAvgValue[2] = hx711.read();
  zeroAvgValue[3] = hx711.read();
  zeroValue = (zeroAvgValue[1] + zeroAvgValue[2] + zeroAvgValue[3]) / 3;
  Serial.println("zeroing");
}

void updateEncoder() {
  int MSB = digitalRead(encA); //MSB = most significant bit
  int LSB = digitalRead(encB); //LSB = least significant bit

  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  lastEncoded = encoded; //store this value for next time
}

void interpretCommand() {

  if (Serial.available() > 0) {
    serialCharBuffer = Serial.read();
    serialNumBuffer = Serial.parseFloat();

    if (serialCharBuffer == 'm') {
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


