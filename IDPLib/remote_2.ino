/*
  Remote line following, with arena awareness.

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

IDPLib IDP;

int lineState[3]; // Last 3 line states

void setup() {
  Serial.begin(9600);
  
  IDP.sensePinLeft = 3; // Front line sensors
  IDP.sensePinRight = 4;
  
  IDP.debugStart("TheBigBlockCarriers", "IDPL209BBC");
  IDP.motorStart();
  IDP.lineStart(IDP.sensePinLeft, IDP.sensePinRight);

  while (!IDP.server.available()) {
    delay(500);
    IDP.send(String(IDP.lineRead()));
  }

  delay(1000);
  IDP.send("Connected.");
  IDP.send("3");
  delay(1000);
  IDP.send("2");
  delay(1000);
  IDP.send("1");
  delay(1000);
  
  lineState[0] = IDP.lineRead();
  if (lineState[0] != 0) {
    IDP.send("Manual reset needed: Both sensors must start off the line");
    while (true) {
      delay(10); // Hang
    } 
  }
  lineState[1] = 0;
  lineState[2] = 0;

  IDP.send("Starting run...");
}

void loop() {
  delay(5); // Small settling time
  // Shift last 3 states and update
  lineState[2] = lineState[1];
  lineState[1] = lineState[0];
  lineState[0] = IDP.lineRead();
  IDP.send(String(lineState[0]));
  
  switch (lineState[0]) {
  case 0:
    if (lineState[1] == 3) {
      double_count = double_count + 1;
      IDP.send("Line crossed");
      IDP.send(String(double_count));
    }
    IDP.motors[2].run(FORWARD);
    IDP.motors[3].run(FORWARD);
    IDP.motors[2].setSpeed(speed1);
    IDP.motors[3].setSpeed(speed1);
    delay(step);
    IDP.motors[2].setSpeed(0);
    IDP.motors[3].setSpeed(0);
    IDP.motors[2].run(RELEASE);
    IDP.motors[3].run(RELEASE);
    break;
  case 2:
    IDP.motors[2].run(FORWARD);
    IDP.motors[3].run(FORWARD);
    IDP.motors[2].setSpeed(speed2);
    IDP.motors[3].setSpeed(speed1);
    delay(step);
    IDP.motors[2].setSpeed(0);
    IDP.motors[3].setSpeed(0);
    IDP.motors[2].run(RELEASE);
    IDP.motors[3].run(RELEASE);
    break;
  case 1:
    IDP.motors[2].run(FORWARD);
    IDP.motors[3].run(FORWARD);
    IDP.motors[2].setSpeed(speed1);
    IDP.motors[3].setSpeed(speed2);
    delay(step);
    IDP.motors[2].setSpeed(0);
    IDP.motors[3].setSpeed(0);
    IDP.motors[2].run(RELEASE);
    IDP.motors[3].run(RELEASE);
    break;
  case 3: 
    IDP.motors[2].run(FORWARD);
    IDP.motors[3].run(FORWARD);
    IDP.motors[2].setSpeed(speed1);
    IDP.motors[3].setSpeed(speed1);
    delay(step);
    IDP.motors[2].setSpeed(0);
    IDP.motors[3].setSpeed(0);
    IDP.motors[2].run(RELEASE);
    IDP.motors[3].run(RELEASE);
    break;
  case 4:
    IDP.send("Manual reset needed: Error condition in line read.");
    while (true) {
      delay(10); // Hang
    }
    break;
  default:
    break;
  }
}
