// Block finding algorithm 
/* Idea is to count number of crosses it has traversed then pivot around whichever ones we do, 
ultrasonic scan until return reading != 0 then calculate distance and go that way */
// GO SLOW!!!

#include <Adafruit_MotorShield.h>
#include <Arduino_LSM6DS3.h>

const int echoPin = 8; // set ultrasonic output pin
const int triggerPin = 7; // set ultrasonic input pin
const int leftfrontPin = ; // set front left line sensor pin
const int rightfrontPin = ; // set front rigiht line sensor pin

unsigned long duration = 0;
unsigned long timeout = 2500000;
int val;
int dist;
int distance;
int cross;
bool leftDetect;
bool rightDetect;

// WIFI SETUP
char ssid[] = "TheBigBlockCarriers"; // your network SSID (name)
char pass[] = "IDPL209BBC";          // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(23);

//SHIELD SETUP
// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Configure all 4 motors outputs. M3 is front left, M4 is front right.
Adafruit_DCMotor *M1 = AFMS.getMotor(1);
Adafruit_DCMotor *M2 = AFMS.getMotor(2);
Adafruit_DCMotor *M3 = AFMS.getMotor(3);
Adafruit_DCMotor *M4 = AFMS.getMotor(4);
Adafruit_DCMotor Motors[] = {*M1, *M2, *M3, *M4};

void setup () {

    Serial.begin(9600);

    // Initialise IMU
    IMU.begin()
    Serial.println("IMU initialised")

    pinMode(triggerPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(leftfrontPin, INPUT);
    pinMode(rightfrontPin, INPUT);
    Serial.println("Pins initialised")

    Serial.println("Setup complete")

}


void loop () {

}

void pulsegeneration() {    
  
    Serial.println("Sending pulse");
    delay(60);
    digitalWrite(triggerPin, LOW);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(15);
    digitalWrite(triggerPin, LOW);

}

float distancecalculation() {

    // returns distance in cm
    dist = duration / 58

    return dist
    
}

void detect_block() {
    leftDetect = digitalRead(leftfrontPin);
    rightDetect = digitalRead(rightfrontPin);
    if (leftDetect == HIGH && rightDetect == HIGH) {
        count += 1;

        if (count == 2) {
            // robot needs to pivot around cross and spin (limited to range of the box), use rotary encoder to track heading
            // use built in gyro?
            // check edge reading of ultrasonic sensor
            pulsegeneration();
            duration = pulseIn(echoPin, HIGH);

            if (duration < ) {
                distance = distancecalculation();
                // match distance to number of rotations on rotary encoder
            }
            

        }
    }
}

// For bot moving functions, might be better to code for distance rather than time

void go_forward() {

    Motors[2].run(FORWARD);
    Motors[3].run(FORWARD);
    delay(100);
    Motors[2].release();
    Motors[3].release();
}

void go_backward() {

    Motors[2].run(BACKWARD);
    Motors[3].run(BACKWARD);
    delay(100);
    Motors[2].release();
    Motors[3].release();

}

void turn_left() {

    Motors[2].run(FORWARD);
    Motors[3].run(BACKWARD);
    delay(100);
    Motors[2].release();
    Motors[3].release();
}

void turn_right() {

    Motors[2].run(BACKWARD);
    Motors[3].run(FORWARD);
    delay(100);
    Motors[2].release();
    Motors[3].release();

}

float readGyro() {

}

int colour_detection() {

}