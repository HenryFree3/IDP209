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
const int slots = 36;
const float degPerSlot = 360/slots;
float velocity;
float diameter = 100; // Wheel diameter, mm
float wheelSep = 220; // Separation between two wheels, mm
// The above can be used, with two wheels velocities, to
// determine the instantaneous centre of rotation

int counts[100];
float sum = 0;
int i = 0;
int j = 0;

bool encoder;
bool encoderOld;

int leftEncoderPin = 5;
int rightEncoderPin = 7;
float pollRate = 1000.0;
float period = 1/pollRate;

volatile unsigned long time;
volatile unsigned long timeOld;
bool updated = false;

IDPLib IDP;

// ISR cannot be declared inside a class member
ISR(TCB0_INT_vect) {
  encoder = digitalRead(7);
  IDP.test = true;
  TCB0.INTFLAGS = TCB_CAPT_bm;
}

void setup() { 
  IDP.motorStart();
  IDP.encoderStart(leftEncoderPin, rightEncoderPin, pollRate);
  encoder = encoderOld = digitalRead(rightEncoderPin);
  
  IDP.motors[3].run(FORWARD);
  IDP.motors[3].setSpeed(60);

  Serial.begin(9600);
  Serial.println("Ready");
}

void loop() {
  if (IDP.test) {
    i++;
    if (encoder != encoderOld) {
      Serial.println("Encoder move detected after " + String(i) + " polls");
      counts[j] = i;
      encoderOld = encoder;
      i = 0;
      j++;    
    }
    IDP.test = false;
  }
  
  if (j % (sizeof(counts)/sizeof(counts)[0]) == 0) {
    // Determine average time between polls from last readings.
    // Useful for calibrating motor speed output against speed control input.
        j = 0;
        for (int k = 0; k < (sizeof(counts)/sizeof(counts)[0]); k++) {
          sum = sum + counts[k];
        }
        Serial.print("Avg. polls per slot was: ");
        Serial.println(float(sum/(sizeof(counts)/sizeof(counts)[0])));
        sum = 0;
      }
}