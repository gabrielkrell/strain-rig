// #define DEBUG_MODE

#include <Q2HX711.h>
Q2HX711 hx711(A0, A1);
float TICKS_PER_GRAM = 983.3473874; // experimentally determined
long int zeroValue;

static int MOTOR_STEP_PIN = 13;
static int MOTOR_DIR_PIN = 12;
static int LED_HIGH_PIN = 23;
static int LED_LOW_PIN = 25;

#include <TimerThree.h>
static double stepperPosCM, desiredPosCM;
volatile long int stepperPosTicks;
         long int desiredPosTicks;
// tick constants: 10 mm / cm, 1 revolution / 4 mm linear movement,
//                 200 steps / 1 revolution, 8 microsteps / 1 step
static int    TICKS_PER_CM = 4000; // 10/4*200*8
static double CMS_PER_TICK = 0.0025; // (1/4000)
static int TURNAROUND_DELAY_CYCLES = 1000; // cycles to wait before reversing

unsigned long timeStarted = 0;

void setup() {
  pinMode(MOTOR_STEP_PIN, OUTPUT);
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  pinMode(LED_HIGH_PIN, OUTPUT);
  pinMode(LED_LOW_PIN, OUTPUT);
  Serial.begin(115200);
  delay(250);
  zeroScale();
  Timer3.initialize(800); // in micros; max speed is 80
  Timer3.attachInterrupt(motorISR);
}

void loop() {
  // get variables first to avoid serial buffer problems, interrupts
  unsigned long time_ = timeStarted > 0 ? micros() - timeStarted : 0;
  long int ticks = stepperPosTicks; // copy volatile var
  double force = readForce(zeroValue);
  stepperPosCM = .00025 * ticks;
  
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
  static int turnaround_delay = 0;
  if (turnaround_delay > 0) {
    turnaround_delay--; // wait a bit before turning around
    return;
  }
  
  static int nextTick;
  if (stepperPosTicks < desiredPosTicks) {
    if (nextTick == -1) {turnaround_delay = TURNAROUND_DELAY_CYCLES;}
    nextTick = 1;
  } else if (stepperPosTicks > desiredPosTicks) {
    if (nextTick == 1) {turnaround_delay = TURNAROUND_DELAY_CYCLES;}
    nextTick = -1;
  } else {
    nextTick = 0;
    digitalWrite(LED_HIGH_PIN, LOW); // we're done
    return;
  }

  digitalWrite(MOTOR_DIR_PIN, nextTick == -1); // apply direction

  static int motorState = HIGH;
  motorState = !motorState;
  digitalWrite(MOTOR_STEP_PIN, motorState); // flip clock
  if (motorState) {stepperPosTicks += nextTick;} // on rising edge, record change
}

float readForce(long int zeroValue) {
  long int sensorVal = hx711.read() - zeroValue;
  return (float)(sensorVal / TICKS_PER_GRAM);
}

void zeroScale() {
  zeroValue = (hx711.read() + hx711.read() + hx711.read())/3;
  Serial.println("Zeroed scale.");
}

void interpretCommand() {
  if (Serial.available() > 0) {
    char letter = Serial.read();
    switch(letter) {
      case 'm':
      case 'M': {
        if (!Serial.available()) {
          break; // no distance sent
        }
        double pos = Serial.parseFloat();
        if (!timeStarted) {
          // on first move, update start time
          timeStarted = micros();
        }
        digitalWrite(LED_HIGH_PIN, HIGH);
        desiredPosCM += pos;
        desiredPosTicks = TICKS_PER_CM * desiredPosCM;
        break;
      }
      case 'z':
      case 'Z':
        zeroScale();
    }
  }
}

