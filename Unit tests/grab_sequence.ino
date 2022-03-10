/*
  Servo sequence for testing mechanics of grabber
*/

#include <Servo.h>

Servo closeServo;  // create servo object to control a servo
Servo liftServo;
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
int liftAng1 = 30;
int liftAng2 = 150;
int closeAng1 = 35;
int closeAng2 = 155;

void setup() {
  closeServo.attach(10);  // attaches the servo on pin 9 to the servo object
  liftServo.attach(9);
  Serial.begin(9600);

  closeServo.write(closeAng2);
  liftServo.write(liftAng2);
}

void loop() {
  Serial.println("Opening");
  for (pos = closeAng2; pos >= closeAng1; pos -= 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    closeServo.write(pos);
    // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15 ms for the servo to reach the position
  }
  delay(1000);

  Serial.println("Closing");
  for (pos = closeAng1; pos <= closeAng2; pos += 1) { // goes from 180 degrees to 0 degrees
    closeServo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15 ms for the servo to reach the position
  }
  delay(1000);

  Serial.println("Lifting");
  for (pos = liftAng2; pos >= liftAng1; pos -= 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    liftServo.write(pos);
    // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15 ms for the servo to reach the position
  }
  delay(1000);
  Serial.println("Lowering");
  for (pos = liftAng1; pos <= liftAng2; pos += 1) { // goes from 180 degrees to 0 degrees
    liftServo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15 ms for the servo to reach the position
  }
  delay(1000);
}
