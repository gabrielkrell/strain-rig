// #define DEBUG_MODE

#include <Q2HX711.h>
Q2HX711 hx711(A0, A1);
float TICKS_PER_GRAM = 720000 / 50; // experimentally determined? 720k = 50g
long int zeroValue;

static int MOTOR_STEP_PIN = 13;
static int MOTOR_DIR_PIN = 12;

#include <TimerThree.h>
static double stepperPosCM, desiredPosCM;
volatile long int stepperPosTicks;
         long int desiredPosTicks;
// tick constants: 10 mm / cm, 1 revolution / 4 mm linear movement,
//                 200 steps / 1 revolution, 8 microsteps / 1 step
static int    TICKS_PER_CM = 4000; // 10/4*200*8
static double CMS_PER_TICK = 0.0025; // (1/4000)

static int loadA = A2;
static int loadB = A3;

double serialNumBuffer = 0;
char serialCharBuffer = 'X';

unsigned long timeStarted = 0;

void setup() {
  pinMode(MOTOR_STEP_PIN, OUTPUT);
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  Serial.begin(115200);
  delay(250);
  zeroScale();
  Timer3.initialize(80);
  Timer3.attachInterrupt(motorISR);
}

void loop() {
  // get variables all at once to avoid waiting for serial buffer to clear
  unsigned long time_ = micros() - timeStarted;
  long int ticks = stepperPosTicks; // copy volatile var
  double force = readForce(zeroValue);
  updateStepperPos();
  
  Serial.print("Time (micros): ");
  Serial.print(time_);
  Serial.print(" Position (cm): ");
  Serial.print(stepperPosCM);
  Serial.print(" Force (g): ");
  Serial.print(force);
  #ifdef DEBUG_MODE
    Serial.print(" Target Pos. (cm): ");
    Serial.print(desiredPosCM);
    Serial.print(" Ticks: ");
    Serial.print(ticks);
    Serial.print(" Target Ticks: ");
    Serial.println(desiredPosTicks);
  #endif
  Serial.println();

  interpretCommand();
}

void motorISR() {
  static int motorState = LOW;
  static int nextTick;
    return;
  }
  // choose next direction (if needed) or exit (if not)
  if (stepperPosTicks < desiredPosTicks) {
    nextTick = 1;
  } else if (stepperPosTicks > desiredPosTicks) {
    nextTick = -1;
  } else {
    nextTick = 0;
    return;
  }
  digitalWrite(MOTOR_DIR_PIN, nextTick == 1); // apply direction
  motorState = !motorState;
  digitalWrite(MOTOR_STEP_PIN, motorState); // flip clock
  if (motorState) { // on rising edge, record change
    stepperPosTicks += nextTick;
  }
}

float readForce(long int zeroValue) {
  long int sensorVal = hx711.read() - zeroValue;
  return (float)(sensorVal / TICKS_PER_GRAM);
}

void zeroScale() {
  zeroValue = (hx711.read() + hx711.read() + hx711.read())/3;
  Serial.println("zeroing");
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
      desiredPosCM = serialNumBuffer;
      desiredPosTicks = TICKS_PER_CM * desiredPosCM;
      serialCharBuffer = 'X';
    }
    else if (serialCharBuffer == 'z') {
      zeroScale();
    }
  }
}

void updateStepperPos() {
  stepperPosCM = .00025 * stepperPosTicks;
}

