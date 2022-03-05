/*
  Optical encoder test
 */

#include <Arduino.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <Adafruit_MotorShield.h>
#include "IDPLib.h"

int step = 50; // Milliseconds between inputs
int speed1 = 230; // Motor speed, 0-255, for forwards
int speed2 = 115; // Reduced motor speed, for one side when turning

int double_count = 0;

float count = 0;
const int slots = 36;
const float degPerSlot = 360/slots;
float velocity;
float diameter = 100; // Wheel diameter, mm

float velArray[slots];
float sum = 0;
int i = 0;

IDPLib IDP;

volatile unsigned long time;
volatile unsigned long timeOld;
bool updated = false;

void setup() {
  IDP.motorStart();
  IDP.motors[3].run(FORWARD);
  IDP.motors[3].setSpeed(30);
  Serial.begin(9600);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  Serial.println("Ready");
  attachInterrupt(7, encoderChange, CHANGE);
  timeOld = 0;
}

void loop() {
  if (updated) {
    velocity = 3.14159*diameter/(slots*(time-timeOld)*1E-6);
    timeOld = time;
    Serial.print("Velocity is: ");
    Serial.print(velocity);
    Serial.println(" mm/s");
    velArray[i] = velocity;
    i++;
    updated = false;

    if (i % (slots-1) == 0) {
      i = 0;
      for (int j = 0; j <= slots - 1; j++) {
        sum = sum + velArray[j];
      }
      Serial.print("Avg. vel over last revolution was: ");
      Serial.print(float(sum/slots));
      Serial.println(" mm/s");
      sum = 0;
    }
  }
}

void encoderChange() {
  time = micros();
  updated = true;
}
