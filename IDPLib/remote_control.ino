/*
  WiFi acess point based debugging.
  Based on Tom Igoe, Nov 2012, adapted by Adafruit.

  Hosts a secure WiFi access point using the on-board chipset.
  You can connect to this access point (192.168.4.1) with
  telnet to pass data to the Arduino.
 */

#include <Arduino.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <Adafruit_MotorShield.h>
#include "IDPLib.h"

int step = 50; // Milliseconds between inputs
int speed = 140; // Motor speed, 0-255
char thisChar;

IDPLib IDP;

void setup() {
  Serial.begin(9600);
  IDP.debugStart("TheBigBlockCarriers", "IDPL209BBC");
  IDP.motorStart();
}

void loop() {
  IDP.refresh(); // Check for new command
  delay(1);     // Minimum loop time for well-behaved wireless communications
  if (IDP.newOut) { // If new output from PC is available
    Serial.print(IDP.out);
    thisChar = IDP.out[0]; // Only care about first char for WASD control
  }
    // Move motors according to WASD control
    switch (thisChar) {
    case 'W':
      IDP.send("MOVING FORWARD");
      IDP.motors[2].run(FORWARD);
      IDP.motors[3].run(FORWARD);
      IDP.motors[2].setSpeed(speed);
      IDP.motors[3].setSpeed(speed);
      delay(step);
      IDP.motors[2].setSpeed(0);
      IDP.motors[3].setSpeed(0);
      IDP.motors[2].run(RELEASE);
      IDP.motors[3].run(RELEASE);
      break;
    case 'A':
      // TURN LEFT
      IDP.send("TURNING LEFT");
      IDP.motors[2].run(FORWARD);
      IDP.motors[3].run(BACKWARD);
      IDP.motors[2].setSpeed(speed);
      IDP.motors[3].setSpeed(speed);
      delay(step);
      IDP.motors[2].setSpeed(0);
      IDP.motors[3].setSpeed(0);
      IDP.motors[2].run(RELEASE);
      IDP.motors[3].run(RELEASE);
      break;
    case 'S':
      // GO BACK
      IDP.send("MOVING BACK");
      IDP.motors[2].run(BACKWARD);
      IDP.motors[3].run(BACKWARD);
      IDP.motors[2].setSpeed(speed);
      IDP.motors[3].setSpeed(speed);
      delay(step);
      IDP.motors[2].setSpeed(0);
      IDP.motors[3].setSpeed(0);
      IDP.motors[2].run(RELEASE);
      IDP.motors[3].run(RELEASE);
      break;
    case 'D':
      // TURN RIGHT
      IDP.send("TURNING RIGHT");
      IDP.motors[2].run(BACKWARD);
      IDP.motors[3].run(FORWARD);
      IDP.motors[2].setSpeed(speed);
      IDP.motors[3].setSpeed(speed);
      delay(step);
      IDP.motors[2].setSpeed(0);
      IDP.motors[3].setSpeed(0);
      IDP.motors[2].run(RELEASE);
      IDP.motors[3].run(RELEASE);
      break;
    default:
      break;
  }
}