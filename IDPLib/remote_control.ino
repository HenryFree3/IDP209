/*
  WiFi acess point based debugging.
  Based on Tom Igoe, Nov 2012, adapted by Adafruit.

  Hosts a secure WiFi access point using the on-board chipset.
  You can connect to this access point (192.168.4.1) with
  telnet to pass data to the Arduino.
 */

#include <SPI.h>
#include <WiFiNINA.h>
#include <Adafruit_MotorShield.h>
#include "IDPLib.h"

int step = 100; // Milliseconds between inputs

IDPLib IDP;

void setup() {
  IDP.debugStart("TheBigBlockCarriers", "IDPL209BBC");
  IDP.motorStart();
}

void loop()
{
    IDP.refresh(); // Check for new command
    delay(10); // Minimum loop time for well-behaved wireless communications
    if (IDP.newOut) { // If new output from PC is available
      Serial.print(IDP.out);
      char thisChar = IDP.out[0]; // Only care about first char for WASD control
      // Move motors to WASD control
      switch (thisChar) {
        case 'W':
          IDP.send("MOVING FORWARD");
          IDP.motors[0].run(FORWARD);
          IDP.motors[3].run(FORWARD);
          IDP.motors[2].setSpeed(255);
          IDP.motors[3].setSpeed(255);
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
          IDP.motors[2].setSpeed(255);
          IDP.motors[3].setSpeed(255);
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
          IDP.motors[2].setSpeed(255);
          IDP.motors[3].setSpeed(255);
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
          IDP.motors[2].setSpeed(255);
          IDP.motors[3].setSpeed(255);
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
}