/* 
    Troubleshooting for ultrasonic ranging sensor.
*/

#include <Arduino.h>

const int echoPin = 8; // set input pin
const int triggerPin = 7; // set output pin

unsigned long duration = 0;
unsigned long timeout = 2500000;
int val;
int dist;
int distance;

int pulseSpacing = 60; // Time between pulses, milliseconds
int pulseLen = 15; // Pulse duration, microseconds 

void setup() {
    // Start serial for output, pin setup
    Serial.begin(9600);
    pinMode(triggerPin, OUTPUT);
    pinMode(echoPin, INPUT);
    Serial.println("Pulse duration is");
    Serial.println(duration);
}

void loop() {
    // Continuously report distance
    Serial.println("Alive"); // Check program hasn't hung
    pulseGeneration();
    duration = pulseIn(echoPin, HIGH);
    distance = distancecalculation();
    Serial.print("Distance in cm is");
    Serial.println(distance);
}

void pulseGeneration() {    
    //Serial.println("Sending pulse");
    delay(pulseSpacing);
    digitalWrite(triggerPin, LOW);
    digitalWrite(triggerPin, HIGH);
    //Serial.println("pulse high");
    //Serial.println(digitalRead(trigger));
    delayMicroseconds(pulseLen);
    digitalWrite(triggerPin, LOW);
    //Serial.println("pulse low");
    //Serial.println(digitalRead(trigger));
}

void distancecalculation() {
    // Convert received duration to distance, cm
    dist = duration / 58;
}
