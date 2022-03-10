/* 
First competition 
*/

#include <Adafruit_MotorShield.h>
#include <Servo.h>


//DEFINE VARIABLES

const int colourPin = ;
const int leftLinePin = 3;
const int rightLinePin = 4; 
int colour = 0;
int cross;
int pos = 0;    // variable to store the servo position
int liftAng1 = 30;
int liftAng2 = 150;
int closeAng1 = 35;
int closeAng2 = 155;

//CREATE SERVO OBJECTS
Servo closeServo;  // create servo object to control a servo
Servo liftServo;

//SHIELD SETUP
// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Configure all 4 motors outputs. M2 is front left, M1 is front right.
Adafruit_DCMotor *M1 = AFMS.getMotor(1);
Adafruit_DCMotor *M2 = AFMS.getMotor(2);
Adafruit_DCMotor *M3 = AFMS.getMotor(3);
Adafruit_DCMotor *M4 = AFMS.getMotor(4);
Adafruit_DCMotor Motors[] = {*M1, *M2, *M3, *M4};

void setup() {

    // Serial initialisation
    Serial.begin(9600);
    while (!Serial)
    {
    ; // wait for serial port to connect. Needed for native USB port only
    }

    // Motor shield initialisation
    if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
    }
    Serial.println("Motor Shield found, proceeding to acess point setup.");

    // Motors initialisation
    Motors[0].setSpeed(128);
    Motors[1].setSpeed(128);

    // Servo initialisation
    closeServo.attach(10);  // attaches the servo on pin 9 to the servo object
    liftServo.attach(9);
    Serial.begin(9600);

    closeServo.write(closeAng2);
    liftServo.write(liftAng2);

    //Pin initialisation
    pinMode(colourPin, INPUT_PULLUP);
    pinMode(leftLinePin, INPUT_PULLUP);
    pinMode(rightLinePin, INPUT_PULLUP);
}

void loop() {
    run();
}

int colour_detection() {

    if (digitalRead(colourPin) == HIGH) {
        
        colour = 1;
        Serial.println("Detected blue");
    }

    else if (digitalRead(colourPin) == LOW) {

        colour = 0;
        Serial.println("Detected red");
    }

    return colour
}

void line_following() {
    
    leftDetect = digitalRead(leftLinePin);
    rightDetect = digitalRead(rightLinePin);

    if (leftDetect == LOW && rightDetect == LOW) {

        Serial.println("Both sensors are now low. GO STRAIGHT.");
        go_forward(100);
  }

    else if (leftDetect == HIGH && rightDetect == HIGH) {

        Serial.println("Both sensors are now low. GO STRAIGHT.");
        go_forward(100);
        cross++;
    }

    else if (leftDetect == HIGH && rightDetect == LOW) {

        Serial.println("Left is high, right is low. TURN LEFT.");
        turn_left(100);       
  }

    else if (leftDetect == LOW && rightDetect == HIGH) {

        Serial.println("Left is low, right is high. TURN RIGHT.");
        turn_right(100);     
  }
}

void open_grabber() {

    Serial.println("Opening");
    for (pos = closeAng2; pos >= closeAng1; pos -= 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    closeServo.write(pos);
    // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15 ms for the servo to reach the position
    }
    delay(1000);
}

void close_grabber() {

    Serial.println("Closing");
    for (pos = closeAng1; pos <= closeAng2; pos += 1) { // goes from 180 degrees to 0 degrees
    closeServo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15 ms for the servo to reach the position
    }
    delay(1000);
}

void lift_grabber() {

    Serial.println("Lifting");
    for (pos = liftAng2; pos >= liftAng1; pos -= 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    liftServo.write(pos);
    // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15 ms for the servo to reach the position
    }
}

void lower_grabber() {

    Serial.println("Lowering");
    for (pos = liftAng1; pos <= liftAng2; pos += 1) { // goes from 180 degrees to 0 degrees
    liftServo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15 ms for the servo to reach the position
    }
}

void turn_right(int time) {

    Motors[0].run(FORWARD);
    Motors[1].run(BACKWARD);
    delay(time);
    Motors[0].run(RELEASE);
    Motors[1].run(RELEASE);
}

void go_forward(int time) {

    Motors[0].run(FORWARD);
    Motors[1].run(FORWARD);
    delay(time);
    Motors[0].run(RELEASE);
    Motors[1].run(RELEASE);
}

void turn_left(int time) {

    Motors[0].run(BACKWARD);
    Motors[1].run(FORWARD);
    delay(time);
    Motors[0].run(RELEASE);
    Motors[1].run(RELEASE);
}

void go_backward(int time) {

    Motors[0].run(BACKWARD);
    Motors[1].run(BACKWARD);
    delay(time);
    Motors[0].run(RELEASE);
    Motors[1].run(RELEASE);
}

void stop() {

    Motors[0].run(RELEASE);
    Motors[1].run(RELEASE);
}

void turn_180() {

    Motors[0].run(BACKWARD);
    Motors[1].run(FORWARD);
    delay(); // test for time
    Motors[0].run(RELEASE);
    Motors[1].run(RELEASE);
}

void run() {
    switch (stage) {
        case 0:
        cross = 0;
        if (cross<=3) {

            line_following();
        }

        if (cross == 3) {
            stop();
            go_forward(); // time pending
            stage++;
        }

        case 1:
        colour_detection();
        stage++;

        case 2:
        open_grabber();
        go_forward();
        stage++;

        case 3:
        close_grabber();
        stage++;

        case 4:
        go_backward();
        stage++;

        case 5:
        stop();


        /*case 5:
        if (cross == 1) {
            line_following();
        }

        else if (cross == 2) {
            stop();
            if (colour == 1) {
                turn_left(); //time pending
                go_forward(); //time pending
                turn_left(); //time pending
                go_forward(); //time pending
                stage++;
            }

            else if (colour == 0) {
                turn_right(); //time pending
                go_forward(); //time pending
                turn_right(); //time pending
                go_forward(); //time pending
                stage++;
            }
        }

        case 6:
        open_grabber();
        stage++;

        case 7:
        go_backward(); // time pending
        stage++;

        case 8:
        close_grabber();
        stage++;

        case 9:
        go_backward();
        stage++();

        case 10:
        if (colour == 1) {
                go_backward(); //time pending
                turn_right(); //time pending
                turn_left(); //time pending
                go_forward(); //time pending
                stage++;
            }

            else if (colour == 0) {
                turn_right(); //time pending
                go_forward(); //time pending
                turn_right(); //time pending
                go_forward(); //time pending
                stage++;
            }*/





        

        

    }
}



        




