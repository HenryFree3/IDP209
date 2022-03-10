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

int countsLeft[slots];
int countsRight[slots];
float sumLeft = 0;
float sumRight = 0;
int i = 0;
int j = 0;
int k = 0;
int l = 0;

bool encoderLeftRead;
bool encoderLeftOld;
bool encoderRightRead;
bool encoderRightOld;

int leftEncoderPin = 5;
int rightEncoderPin = 7;

float pollRate = 10000.0; // Interrupt rate
float period = 1/pollRate;

volatile unsigned long time;
volatile unsigned long timeOld;
bool updated = false;

IDPLib IDP;

// For speed constraint
int minSpeed = 60; // Motors don't turn consistently below this
int maxSpeed = 255; // 8 bit maximum

int stepTargetStatic = slots*40;
int baseSpeed = 0;
float kp = 0.1;

int leftCount = 0;
int rightCount = 0;

int leftSpeed = baseSpeed;
int rightSpeed = baseSpeed;

char leftDir = 'f';
char rightDir = 'f';

int leftSpeedTarget;
int rightSpeedTarget;

float pTermLeft;
float pTermRight;


// ISR cannot be declared inside a class member
ISR(TCB0_INT_vect) {
  encoderLeftRead = digitalRead(leftEncoderPin);
  encoderRightRead = digitalRead(rightEncoderPin);
  
  if (encoderLeftRead != encoderLeftOld) {
      if (leftDir == 'f') {
        leftCount++;
      }
      else {
        leftCount--;
      }
      encoderLeftOld = encoderLeftRead;
  }
  if (encoderRightRead != encoderRightOld) {
      if (rightDir == 'f') {
        rightCount++;
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
  IDP.motorStart();
  delay(1000);
  IDP.encoderStart(leftEncoderPin, rightEncoderPin, pollRate);
  encoderLeftRead = encoderLeftOld = digitalRead(leftEncoderPin);
  encoderRightRead = encoderRightOld = digitalRead(rightEncoderPin);
  
  IDP.motors[3].run(FORWARD);
  IDP.motors[3].setSpeed(baseSpeed);
  IDP.motors[2].run(FORWARD);
  IDP.motors[2].setSpeed(baseSpeed);
}

void loop() {
  if (leftCount != stepTargetStatic) {
    pTermLeft = kp * (stepTargetStatic - leftCount);
    if (pTermLeft < 0) {
      if (leftDir == 'f') {
        leftDir = 'b';
        IDP.motors[2].run(RELEASE);
        IDP.motors[2].run(BACKWARD);
      }
      leftSpeedTarget = round(baseSpeed - pTermLeft);
    }
    else {
      if (leftDir == 'b') {
        leftDir = 'f';
        IDP.motors[2].run(RELEASE);
        IDP.motors[2].run(FORWARD);
      }
      leftSpeedTarget = round(baseSpeed + pTermLeft);
    }
    leftSpeed = constrain(leftSpeedTarget, minSpeed, maxSpeed);
    IDP.motors[2].setSpeed(leftSpeed);
    i++;
  }

  if (rightCount != stepTargetStatic) {
    pTermRight = kp * (stepTargetStatic - rightCount);
    if (pTermRight < 0) {
      if (rightDir == 'f') {
        rightDir = 'b';
        IDP.motors[3].run(RELEASE);
        IDP.motors[3].run(BACKWARD);
      }
      rightSpeedTarget = round(baseSpeed - pTermRight);
    }
    else {
      if (rightDir == 'b') {
        rightDir = 'f';
        IDP.motors[3].run(RELEASE);
        IDP.motors[3].run(FORWARD);
      }
      rightSpeedTarget = round(baseSpeed + pTermRight);
    }
    rightSpeed = constrain(rightSpeedTarget, minSpeed, maxSpeed);
    IDP.motors[3].setSpeed(rightSpeed);
    j++;
  }
  if (leftCount == stepTargetStatic) {
    IDP.motors[2].setSpeed(0);
    Serial.print("Done left: ");
    Serial.println(leftCount);
    delay(1000);
  }
    if (rightCount == stepTargetStatic) {
    IDP.motors[3].setSpeed(0);
    Serial.print("Done right: ");
    Serial.println(rightCount);
    delay(1000);
  }
  if ((i % 1000) == 0) {
    Serial.print("Current left speed: ");
    Serial.println(leftSpeed);
    Serial.print("Remaining count left: ");
    Serial.println(stepTargetStatic - leftCount);
  }
  if ((j % 1000) == 0) {
    Serial.print("Current right speed: ");
    Serial.println(rightSpeed);
    Serial.print("Remaining count right: ");
    Serial.println(stepTargetStatic - rightCount);
  }  
}
