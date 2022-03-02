/*
    Troubleshooting for single line sensor.
    Assumes an amplifier and comparator circuit
    are connected to the sensor to  produce a
    digital input. 
*/

#include <Arduino.h>

const int pin = 3;

void setup() {
    pinMode(pin, INPUT);
    Serial.begin(9600);
}

void loop() {
    if (digitalRead(pin) == LOW) {
        Serial.println("Detected LOW");
        delay(250);
    }
    else if (digitalRead(pin) == HIGH) {
        Serial.println("Detected HIGH");
        delay(250);
    }
    else {
        Serial.println("Read failed");
    }
}
