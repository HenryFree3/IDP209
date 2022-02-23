/*
This is a test sketch for the Adafruit assembled Motor Shield for Arduino v2
*/

#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *M1 = AFMS.getMotor(1);
Adafruit_DCMotor *M2 = AFMS.getMotor(2);
Adafruit_DCMotor *M3 = AFMS.getMotor(3);
Adafruit_DCMotor *M4 = AFMS.getMotor(4);

Adafruit_DCMotor Motors[] = {*M1, *M2, *M3, *M4};

int step = 1;
String command;

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("DC Motor test");

  if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }
  Serial.println("Motor Shield found.");
}

void loop() {
  if (Serial.available() > 0) {
    command = Serial.readString()[0]; // Get first character from buffer
    Serial.println((String)"Received:"+command);
  }

  if (command == "1") {
    for (int i = 0; i<4; i++)
    {
    Serial.println((String)"Beginning motor test: M"+(i+1));
    Motors[i].run(FORWARD);
    for (int j=0; j<255; j++) {
      Motors[i].setSpeed(j);
      delay(step);
    }
    for (int j=255; j!=0; j--) {
      Motors[i].setSpeed(j);
      delay(step);
    }
    Motors[i].run(RELEASE);
    Serial.println((String)"Ending motor test: M"+(i+1));
    }
  }
}
