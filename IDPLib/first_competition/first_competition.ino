/*
    First competition integration
*/

// Use switch...case structure to ensure algorithm flow

#include "IDPLIb.h"

// Sensor and pin declarations
const int leftLinePin = 4;
const int rightLinePin = 3;
int lineState; int lineStateOld;

const int leftEncoderPin = 13;
const int rightEncoderPin = 7;
int leftCount = 0;
int rightCount = 0;
bool leftEncRead; bool leftEncReadOld;
bool rightEncRead; bool rightEncReadOld;
const int encAvgWindow = 5; // Length of moving average window for encoder velocity
uint32_t leftEncPolls; uint32_t leftEncSum; uint32_t leftEncPollsArray[encAvgWindow]; float leftPollsAvg;
uint32_t rightEncPolls; uint32_t rightEncSum; uint32_t rightEncPollsArray[encAvgWindow]; float rightPollsAvg;
int countError = 0;
float leftVel;
float rightVel;
char leftDir = 'f';
char rightDir = 'f';

const int liftServoPin = 9;
const int closeServoPin = 10;
const int liftAng1 = 50; const int liftAng2 = 165;
const int grabAng1 = 20; const int grabAng2 = 150;

const int ledPin = 11;
const int colourPin = 12;
const int irPin = A3;

// PD control
float pTerm; float dTerm;
float kp = 2.5;
float kd = 0.7;
float leftTarget; int leftSpeed;
float rightTarget; int rightSpeed;
float speedOffset;

// Timing configuration
int loopTimeLast;
int intRate = 10000; // Sensor interrupt timing rate, Hz

// Key geometry
const int slots = 120; // Code wheel slots per revolution
const float wheelDia = 104; // Wheel diameter, mm
const float wheelSep = 244; // Separation between wheel mid-thicknesses, mm
const float pi = 3.141593;
const float wheelTravel = pi*wheelDia;
const float velConst = pi * wheelDia * intRate/slots;

// Motor configuration
const int baseSpeed = 230;
const int minSpeed = 100;
const int maxSpeed = 255;
const int turnSpeedOffset = 25;

// Telemetry configuration
int updateRate  = 4;
int stateID = 0;
int stage = 0;

IDPLib IDP; // Create instance of IDPLib class

int waits = 1;
int doubleLineCount = 0;

// ISR cannot be declared inside a class member
ISR(TCB0_INT_vect) {
  leftEncRead = digitalRead(leftEncoderPin);
  rightEncRead = digitalRead(rightEncoderPin);
  leftEncPolls++;
  rightEncPolls++;
  
  if (leftEncRead != leftEncReadOld) {
      if (leftDir == 'f') {
        leftCount++;
        leftEncSum += leftEncPolls - leftEncPollsArray[encAvgWindow-1]; // Update sum, saves a second for loop line to sum array
        for (int i = encAvgWindow -1; i > 0; i--) {
          leftEncPollsArray[i] = leftEncPollsArray[i-1]; // Shift array right
        }
        leftEncPollsArray[0] = leftEncPolls;
        leftEncPolls = 0;
        leftPollsAvg = leftEncSum / encAvgWindow; // Moving average
      }
      else {
        leftCount--;
      }
      leftEncReadOld = leftEncRead;
  }
  if (rightEncRead != rightEncReadOld) {
      if (rightDir == 'f') {
        rightCount++;
        rightEncSum += rightEncPolls - rightEncPollsArray[encAvgWindow-1]; // Update sum, saves a second for loop line to sum array
        for (int j = encAvgWindow -1; j > 0; j--) {
          rightEncPollsArray[j] = rightEncPollsArray[j-1]; // Shift array right
        }
        rightEncPollsArray[0] = rightEncPolls;
        rightEncPolls = 0;
        rightPollsAvg = rightEncSum / encAvgWindow; // Moving average
      }
      else {
        rightCount--;
      }
      rightEncReadOld = rightEncRead;
  }

  lineStateOld = lineState;
  lineState = IDP.lineRead();

  TCB0.INTFLAGS = TCB_CAPT_bm;
}

// Setup, initialise, check sensors, setup network, everything that should only run once
void setup() {
    delay(500);
    Serial.begin(9600);
    delay(500);
    IDP.debugStart("TheBigBlockCarriers", "IDPL209BBC");

    IDP.lineStart(leftLinePin, rightLinePin);
    //IDP.encoderStart(leftEncoderPin, rightEncoderPin);
    IDP.irStart(irPin);
    IDP.colourStart(colourPin);
    IDP.motorStart();

    pinMode(ledPin, OUTPUT);
}
void loop () {
    // Listen for commands from python here
    // send telemetry at regular intervals (measure loop time), if stage > 0

    switch (stage) {     
        case 0:
            // In startup
            waits = 1;
            IDP.send("\nCASE 0: PRE-START COMPLETE.\n\n");
            IDP.refresh();
            while (!IDP.connected) {
                IDP.send("AWAITING CONNECTION... (" + (String) waits + ")\n");
                waits++;
                delay(5000);
                IDP.refresh();
            }
            stage = 1;
            break;

        case 1: // Most of the code, for timing telemetry, checking for a start (or stop) command or a command to test a new stage should run through all stages. What's the best way to integrate it with the loop / switch ... case?
        // Connection received, start receiving telemetry data, wait for go signal
            waits = 1;
            IDP.send("\nCASE 1: CLIENT CONNECTED, STARTING TELEMETRY STREAM AT " + (String) IDP.updateRate + " Hz.\n\n");
            IDP.telemStart();
            while (!IDP.startFlag) {
                if (waits % 10 == 0) {
                    IDP.send("AWAITING START COMMAND... (");
                    IDP.send((String) waits);
                    IDP.send(")\n");
                }
                IDP.refresh();
                if (!IDP.connected) {
                  stage = 0;
                  break;
                }
                if (IDP.out[0] == 'S' && IDP.out[1] == 'T' && IDP.out[2] == 'A' && IDP.out[3] == 'R' && IDP.out[4] == 'T') { // Command starts with "START"
                  char stageRead[2] = {IDP.out[6], IDP.out[7]};
                  stage = 2; // fix this
                  //stage = atoi(stageRead); // The 7th and 8th characters specify the start stage
                  IDP.send((String) stageRead);
                  break;
                }
                waits++;
                if (IDP.telemConnected) {
                  IDP.telemUpdate();
                }
                while ((millis() - IDP.lastTelemTime) < 1000/((float) IDP.updateRate)) {
                    // Sit here until timing is correct
                }
                IDP.lastPeriod = millis() - IDP.lastTelemTime;
                IDP.lastTelemTime = millis();
            }
            //break;

        case 10:
            IDP.send("\nCASE 2: DRIVING TO COLLECTION AREA.\n");
            // Check we're ready to go: both line sensors should be off the line
            if (IDP.lineRead() != 3) { // Change this in morning to '0': both sensors low
              IDP.send("Line sensors must not be on a line when starting. Resetting.\n");
              stage = 1;
            }
            else {
              IDP.send("Line sensor start readings ok.\n");
            }
            IDP.send("Moving servos to starting position.\n");
            IDP.grabServo.write(grabAng1);
            IDP.liftServo.write(liftAng1);
            IDP.send("Driving to collection area.");
         
            IDP.motors[0].run(FORWARD);
            IDP.motors[0].setSpeed(baseSpeed);
            IDP.motors[1].run(FORWARD);
            IDP.motors[1].setSpeed(baseSpeed);

            int driveStartTime = millis();
            IDP.encoderStart(leftEncoderPin, rightEncoderPin);
            int i = 0;
            while (doubleLineCount < 3) {
              i++;
              if (i % 10000) {
                IDP.send("Line state is: " + (String) lineState + "\n");
              }
              if (lineState != 3) { // Change this in morning to 0
                IDP.send("Possible double line or going off the line, driving straight.\n");
                int additionalCounts = 12; // Extra distance to check we haven't reached a double line
                int leftCountCur = leftCount;
                int rightCountCur = rightCount;
                while (leftCount < leftCountCur + additionalCounts &&
                       rightCount < rightCountCur + additionalCounts) {
                         // do nothing, keep going for now
                         delay(50);
                         IDP.send("Still going straight: left: " + (String) leftCount +
                                  " right:" + (String) rightCount + ", originals were: " +
                                  (String) leftCountCur + ", " + (String) rightCountCur + "\n");
                       }
                if (lineState == 0) { // Change this in morning to 3
                  IDP.send("It was a double line, total: " + (String) doubleLineCount + "\n");
                  doubleLineCount += 1;
                }
                else {
                  if (lineState == 1) { // Turn left. Change this in morning to 2.
                    IDP.send("Robot is veering right, turning left.\n");
                    while (lineState == 1) {
                      IDP.motors[0].setSpeed(baseSpeed-turnSpeedOffset); // Set these speed updates to a new variable and clamp then when reviewing
                      IDP.motors[1].setSpeed(baseSpeed+turnSpeedOffset);
                    }
                  }
                  else if (lineState == 2) { // Turn right. Change this in morning to 1.
                    IDP.send("Robot is veering left, turning right.\n");
                    while (lineState == 2) {
                      IDP.motors[0].setSpeed(baseSpeed+turnSpeedOffset); // Set these speed updates to a new variable and clamp then when reviewing
                      IDP.motors[1].setSpeed(baseSpeed-turnSpeedOffset);
                    }
                  }
                    IDP.send("Back on the line.\n");
                    IDP.motors[0].setSpeed(baseSpeed); // Go back to driving straight
                    IDP.motors[1].setSpeed(baseSpeed);
                    leftCount = 0; // Reset encoder counts so proportional error is 0 again
                    rightCount = 0;
                    delay(500); // Give time to recalculate wheel speeds before going back to PD control
                }
                  
              }

              if (millis() - driveStartTime < 500) {
                // don't do PD control yet, let velocity moving averages settle
              }
              else {
                // do PD control
                countError = leftCount - rightCount;
                leftVel = velConst/leftPollsAvg;
                rightVel = velConst/rightPollsAvg;

                pTerm = kp * countError;
                dTerm = kd * (leftVel - rightVel);

                speedOffset = dTerm + pTerm;
                leftTarget = baseSpeed - speedOffset;
                rightTarget = baseSpeed + speedOffset;
                leftSpeed = round(constrain(leftTarget, minSpeed, maxSpeed));
                rightSpeed = round(constrain(rightTarget, minSpeed, maxSpeed));
                IDP.motors[0].setSpeed(leftSpeed);
                IDP.motors[1].setSpeed(rightSpeed);
              }
            }
            IDP.send("3 double lines counted.");

            // Go signal received, drive as straight as possible and count double lines crossed.
            // Check both line sensors are starting low.
            // Reset wheel encoder variables to zero. (or better to just take note of the initial values?)
                // In either case the velocities / poll count arrays should be zeroed, might make a function for this
            
            // Start driving forwards, implement PD control on wheels
            // while driving, check for line sensor changes - if one goes high, wait another (X) rotations of the wheels
                // if they both go high within this many rotations, keep going straight and increment double line count
                // else, we have just drifted off the line: increase the wheel speed on one side - Reset encoder difference?

            // when double line count reaches 3
                // case = 3
          break;

        case 2:
            Serial.println("Made it to case 3");
            //IDP.send("\n CASE 3: DOUBLE LINE COUNT REACHED, DRIVING TO SCAN POINT.\n");
            IDP.motors[0].setSpeed(leftSpeed);
            IDP.motors[1].setSpeed(rightSpeed);
            float scanPointDist = 200; // Distance in mm from last double line
            int scanPointCount = round(scanPointDist*slots/wheelTravel);
            int leftCountCur = leftCount; // Get one wheel's current count to use for 
            delay(500); 
            while (leftCount < leftCountCur + scanPointCount) {
              // do PD control
                countError = leftCount - rightCount;
                leftVel = velConst/leftPollsAvg;
                rightVel = velConst/rightPollsAvg;

                pTerm = kp * countError;
                dTerm = kd * (leftVel - rightVel);

                speedOffset = dTerm + pTerm;
                leftTarget = 
                baseSpeed - speedOffset;
                rightTarget = baseSpeed + speedOffset;
                leftSpeed = round(constrain(leftTarget, minSpeed, maxSpeed));
                rightSpeed = round(constrain(rightTarget, minSpeed, maxSpeed));
                IDP.motors[0].setSpeed(leftSpeed);
                IDP.motors[1].setSpeed(rightSpeed);
            }
            IDP.motors[0].setSpeed(0);
            IDP.motors[1].setSpeed(0);
            IDP.send("Scan point reached.\n");
            stage = 4;


            // Drive a hardcoded distance to the search point using encoders and line following
            // consider using encoders and line sensors to try and precisely align on the centre of the line?
            // case = 4
            break;
        case 4:
          float halfScanAngle = 20;
          float halfScanRots = halfScanAngle / 360;
          float halfScanWheelRots = halfScanRots * wheelDia/wheelSep;
          int halfScanWheelSlots = round(halfScanWheelRots * slots);
          leftCount = 0; rightCount = 0;

          IDP.motors[0].run(RELEASE);
          IDP.motors[0].run(BACKWARD);
          IDP.motors[1].run(RELEASE);
          IDP.motors[1].run(FORWARD);
          IDP.motors[0].setSpeed(baseSpeed);
          IDP.motors[1].setSpeed(baseSpeed);
          delay(500);

          while (leftCount != halfScanWheelSlots && rightCount != halfScanWheelSlots) {
            countError = leftCount - rightCount;
            leftVel = velConst/leftPollsAvg;
            rightVel = velConst/rightPollsAvg;

            pTerm = kp * countError;
            dTerm = kd * (leftVel - rightVel);

            speedOffset = dTerm + pTerm;
            leftTarget = 
            baseSpeed - speedOffset;
            rightTarget = baseSpeed + speedOffset;
            leftSpeed = round(constrain(leftTarget, minSpeed, maxSpeed));
            rightSpeed = round(constrain(rightTarget, minSpeed, maxSpeed));
            IDP.motors[0].setSpeed(leftSpeed);
            IDP.motors[1].setSpeed(rightSpeed);
          }
          IDP.send("Rotation complete.\n");
          

            // Enter search mode
            // Sweep out an arc by rotating some fixed number of degrees on the spot, reading the IR sensor, repeating
            // When the IR sensor reading changes dramatically
                //perform a refind scan around the block
                //if the measured distance and angle through which the object are detected are consistent with the expected block size
                    // case = 5
            IDP.send("Case 4");
          break;

        case 5: 
            // Determine required rotation and distance to block
            // Perform the rotation, then drive forwards to expected 15 cm distance.
            // Reacquire block
            // if reacquire fails
                // Reverse the movements just performed, reacquire line and go again from case 4
          break;

            // else perform fine alignment with colour sensor
        case 6: 
            // Perform colour detection, store the result for later so we know where we need to go
            // Fine alignment with grabber
          break;

        case 7: 
            // Activate block retrieval mechanism
            // if block retrieval fails:
                // Backtrack all movements to case 4, scan again
          break;

        case 8: 
            // Backtrack to line
            // need to modify case 2 code to be reusable at this point - follow line to delivery area
          break;

        case 9: 
            // using knowledge of block colour, any previously retrieved blocks, navigate to correct drop area
          break;

        case 10: 
            // Align with target, deliver block
          break; 

        case 11: 
            // Return to line, repeat cases 4 to 11 until all 4 blocks done
          break;

        case 12: 
            // Return to homebase, potentially implement timer such that robot knows when time is running out and will return to homebase, aborting remaining blocks
          break;
    }
}
