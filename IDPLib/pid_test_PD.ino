/*
  Optical encoder test

  Independent proportional control for both wheels.
 */

#include <Arduino.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <Adafruit_MotorShield.h>
#include "IDPLib.h"

// Wheel geometry
const int slots = 120;
const float degPerSlot = 360/slots;
float velocity;
float diameter = 104; // Wheel diameter, mm
float wheelSep = 244; // Separation between two wheels, mm
// The above can be used, with two wheels velocities, to
// determine the instantaneous centre of rotation

bool encoderLeftRead;
bool encoderLeftOld;
bool encoderRightRead;
bool encoderRightOld;

int leftEncoderPin = 5;
int rightEncoderPin = 6;

int pollRate = 10000; // Interrupt rate

volatile unsigned long time;
volatile unsigned long timeOld;
bool updated = false;

IDPLib IDP;

// For speed constraint
int minSpeed = 60; // Motors don't turn consistently below this
int maxSpeed = 255; // 8 bit maximum

int baseSpeed = 230;

uint32_t leftCount = 0;
uint32_t rightCount = 0;

char leftDir = 'f';
char rightDir = 'f';

float pTermLeft;
float pTermRight;

float iTermLeft;
float iTermRight;

const int windowLen = 5;
uint32_t leftPolls[windowLen];
uint32_t rightPolls[windowLen];

float velConst = 3.14159 * diameter * pollRate/slots;

uint32_t leftSum = 0;
uint32_t rightSum = 0;

uint32_t leftTime;
uint32_t rightTime;

uint32_t leftPollsLast = 0;
uint32_t rightPollsLast = 0;

float leftPollsAvg;
float rightPollsAvg;

// ISR cannot be declared inside a class member
ISR(TCB0_INT_vect) {
  encoderLeftRead = digitalRead(leftEncoderPin);
  encoderRightRead = digitalRead(rightEncoderPin);
  leftPollsLast++;
  rightPollsLast++;
  
  if (encoderLeftRead != encoderLeftOld) {
      if (leftDir == 'f') {
        leftCount++;
        leftSum += leftPollsLast - leftPolls[windowLen-1]; // Update sum, saves a second for loop line to sum array
        for (int i = windowLen -1; i > 0; i--) {
          leftPolls[i] = leftPolls[i-1]; // Shift array right
        }
        leftPolls[0] = leftPollsLast;
        leftPollsLast = 0;
        leftPollsAvg = leftSum / windowLen; // Moving average
      }
      else {
        leftCount--;
      }
      encoderLeftOld = encoderLeftRead;
  }
  if (encoderRightRead != encoderRightOld) {
      if (rightDir == 'f') {
        rightCount++;
        rightSum += rightPollsLast - rightPolls[windowLen-1]; // Update sum, saves a second for loop line to sum array
        for (int j = windowLen -1; j > 0; j--) {
          rightPolls[j] = rightPolls[j-1]; // Shift array right
        }
        rightPolls[0] = rightPollsLast;
        rightPollsLast = 0;
        rightPollsAvg = rightSum / windowLen; // Moving average
      }
      else {
        rightCount--;
      }
      encoderRightOld = encoderRightRead;
  }
  TCB0.INTFLAGS = TCB_CAPT_bm;
}

void setup() { 
  Serial.begin(9600);
  delay(1000);
  Serial.println("Ready");
  IDP.encoderStart(leftEncoderPin, rightEncoderPin, pollRate);
  IDP.motorStart();
  delay(1000);
  encoderLeftRead = encoderLeftOld = digitalRead(leftEncoderPin);
  encoderRightRead = encoderRightOld = digitalRead(rightEncoderPin);
  
  IDP.motors[3].run(FORWARD);
  IDP.motors[3].setSpeed(baseSpeed);
  IDP.motors[2].run(FORWARD);
  IDP.motors[2].setSpeed(baseSpeed);
}

int countErr = 0;
float leftVel;
float rightVel;

float dTerm;
float pTerm;
float kd = 0.25;
float kp = 1;
float offset;
float leftTarget;
float rightTarget;

int leftSpeed;
int rightSpeed;

int m = 0;
void loop() {
  m++;
  countErr = leftCount - rightCount;
  leftVel = velConst/leftPollsAvg;
  rightVel = velConst/rightPollsAvg;
  delay(10);
  if ((m % 100) == 0) {
    Serial.print("Lifetime count difference: ");
    Serial.print(countErr);
    Serial.print(", left wheel velocity: ");
    Serial.print(leftVel);
    Serial.print(" , right wheel velocity: ");
    Serial.println(rightVel);
  
    Serial.print("P term: ");
    Serial.print(pTerm);
    Serial.print(", D term: ");
    Serial.println(dTerm);
  }

  pTerm = kp * countErr;
  dTerm = kd * (leftVel - rightVel);

  offset = dTerm + pTerm;
  leftTarget = baseSpeed - offset;
  rightTarget = baseSpeed + offset;
  leftSpeed = round(constrain(leftTarget, 60, 255));
  rightSpeed = round(constrain(rightTarget, 60, 255));
  IDP.motors[2].setSpeed(leftSpeed);
  IDP.motors[3].setSpeed(rightSpeed);
}
