/*
    Demonstration for sending, receiving, parsing and plotting the robot state vector.
    Furhter optimisation is needed, likely on the Python side, to increase maximum updateRate.
*/

#include <Arduino.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <Adafruit_MotorShield.h>
#include "IDPLib.h"

float updateRate = 4; // Number of updates per second to broadcast. >1.5 is problematic, even on a fast PC, without blitting
int stateID = 0;
// Used if state vectors are only sent on request: this is the request message to look for from the client
char vectorMsg[] = "*** REQUESTING STATE VECTOR ***";
const int bufferLen = 1;
String stateVector;

IDPLib IDP;

const unsigned long loopTime = 1000000/updateRate; // Microseconds
static unsigned long lastLoopTime;
static unsigned long lasterLoopTime;
static unsigned long arTime1;
static unsigned long arTime2;

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
  
  stateVector = String("STATE_BEGIN") + " " +
  String(stateID) + "," + String((lastLoopTime - lasterLoopTime)) + " " +
  String(random(-10, 11)) + "," + String(random(-10, 11)) + " " +
  String(random(-180, 181))+ " " +
  String(random(0, 256)) + "," + String(random(0, 256))+ " " +
  String(random(-50, 51)) + "," + String(random(-25, 26)) + "," + String(random(-75, 76)) + " " +
  String("STATE_END");
  
  IDP.send(stateVector);

  while (micros()-lastLoopTime < loopTime) {
    // Wait for correct timing
  }
  stateID++;
  lasterLoopTime = lastLoopTime;
  lastLoopTime = micros();
}
