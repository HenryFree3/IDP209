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

int step = 50;   // Milliseconds between inputs
int speed = 230; // Motor speed, 0-255
int double_count = 0;
char lineState[3]; // Last 3 line states

IDPLib IDP;

void setup() {
  Serial.begin(9600);

  IDP.sensePinLeft = 3; // Front line sensors
  IDP.sensePinRight = 4;

  IDP.debugStart("TheBigBlockCarriers", "IDPL209BBC");
  IDP.motorStart(false);
  IDP.lineStart(IDP.sensePinLeft, IDP.sensePinRight);

  while (!IDP.server.available())
  {
    delay(500);
    Serial.print("New read:");
    Serial.println(IDP.lineRead());
  }
}

void loop () {
  
}
