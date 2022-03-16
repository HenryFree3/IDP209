/*
  Optical encoder test

  Next step: PID control for matching encoder counts
    i.e. when trying to move forwards, drive the difference
    between the number of encoder steps on the wheels to zero
    by adjusting the motor speeds.
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
int revsL = 0; int revsR = 0;

bool encoderLeftRead;
bool encoderLeftOld;
bool encoderRightRead;
bool encoderRightOld;

int leftEncoderPin = 13;
int rightEncoderPin = 7;

float pollRate = 10000.0;
float period = 1/pollRate;

volatile unsigned long time;
volatile unsigned long timeOld;
bool updated = false;

IDPLib IDP;

// ISR cannot be declared inside a class member
ISR(TCB0_INT_vect) {
  encoderLeftRead = digitalRead(leftEncoderPin);
  encoderRightRead = digitalRead(rightEncoderPin);
  IDP.test = true;
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
  
  IDP.motors[1].run(FORWARD);
  IDP.motors[1].setSpeed(220);
  IDP.motors[0].run(FORWARD);
  IDP.motors[0].setSpeed(220);
}

void loop() {
  encoderLeftRead = digitalRead(leftEncoderPin);
  encoderRightRead = digitalRead(rightEncoderPin);
  if (encoderLeftRead != encoderLeftOld) {
    IDP.test = true;
  }
  if (encoderRightRead != encoderRightOld) {
    IDP.test = true;
  }
  

  
  if (IDP.test) {
    i++;
    k++;
    if (encoderLeftRead != encoderLeftOld) {
      countsLeft[j] = i;
      encoderLeftOld = encoderLeftRead;
      i = 0;
      j++;    
    }
    if (encoderRightRead != encoderRightOld) {
      countsRight[l] = k;
      encoderRightOld = encoderRightRead;
      k = 0;
      l++;
    }
    IDP.test = false;
  }
  
  if (j == (sizeof(countsLeft)/sizeof(countsLeft)[0])) {
    // Determine average time between polls from last readings of left wheel.
    // Useful for calibrating motor speed output against speed control input.
        j = 0;
        revsL = revsL + 1;
        for (int m = 0; m < (sizeof(countsLeft)/sizeof(countsLeft)[0]); m++) {
          sumLeft = sumLeft + countsLeft[m];
        }
        Serial.print("Avg. polls per slot on last left wheel revolution was: ");
        Serial.print(float(sumLeft/(sizeof(countsLeft)/sizeof(countsLeft)[0])));
        Serial.print(", total revolutions: ");
        Serial.println(revsL);
        sumLeft = 0;
      }
      
  if (l == (sizeof(countsRight)/sizeof(countsRight)[0])) {
    // Determine average time between polls from last readings of right wheel.
    // Useful for calibrating motor speed output against speed control input.
        l = 0;
        revsR = revsR + 1;
        for (int n = 0; n < (sizeof(countsRight)/sizeof(countsRight)[0]); n++) {
          sumRight = sumRight + countsRight[n];
        }
        Serial.print("Avg. polls per slot on last right wheel revolution was: ");
        Serial.print(float(sumRight/(sizeof(countsRight)/sizeof(countsRight)[0])));
        Serial.print(", total revolutions: ");
        Serial.println(revsR);
        sumRight = 0;
      }
}
