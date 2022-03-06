/*
    Demonstration for sending, receiving, parsing and plotting the robot state vector.
    Furhter optimisation is needed, likely on the Python side, to increase maximum updateRate.
*/

#include <Arduino.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <Adafruit_MotorShield.h>
#include "IDPLib.h"

float updateRate = 1.5; // Number of updates per second to broadcast. >1.5 is problematic, even on a fast PC
int stateID = 0;

IDPLib IDP;

const unsigned long loopTime = 1000000/updateRate; // Microseconds
static unsigned long lastLoopTime;
static unsigned long lasterLoopTime;

void setup() {
    Serial.begin(9600);
    IDP.debugStart("TheBigBlockCarriers", "IDPL209BBC");
    delay(500);
}

void loop() {
  IDP.refresh();

    // State is given as:
    // Start of state identifier: STATE_BEGIN
    // State count, time for last update to occur (microseconds)
    // Displacement x, displacement y since last update (mm)
    // Rotation of robot, clockwise positive, since last update (degrees)
    // left wheel speed setting, right wheel speed setting (0-255)
    // encoder error correction P, I, D values (dimensionless)
    // End of state identifier: STATE_END

    IDP.send(String("STATE_BEGIN") + " " +
    String(stateID) + "," + String((lastLoopTime - lasterLoopTime)) + " " +
    String(random(-10, 11)) + "," + String(random(-10, 11)) + " " +
    String(random(-180, 181))+ " " +
    String(random(0, 256)) + "," + String(random(0, 256))+ " " +
    String(random(-50, 51)) + "," + String(random(-25, 26)) + "," + String(random(-75, 76)) + " " +
    String("STATE_END"));
    
    stateID++;
    while (micros() - lastLoopTime < loopTime) {
      // Synchronise loop to updateRate by waiting here
    }
    lasterLoopTime = lastLoopTime;
    lastLoopTime = micros();
}
