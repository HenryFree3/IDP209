/*
    First competition integration
*/

// Use switch...case structure to ensure algorithm flow

#include "IDPLIb.h"

// Sensor and pin declarations
const int leftLinePin = 3;
const int rightLinePin = 4;

const int leftEncoderPin = 5;
const int rightEncoderPin = 6;
bool leftEncRead; bool leftEncReadOld;
bool rightEncRead; bool rightEncReadOld;
const int encAvgWindow = 5; // Length of moving average window for encoder velocity
uint32_t leftEncPolls; uint32_t leftEncSum; uint32_t leftEncPollsArray[encAvgWindow]; float leftPollsAvg;
uint32_t rightEncPolls; uint32_t rightEncSum; uint32_t leftEncPollsArray[encAvgWindow]; float rightPollsAvg;

const int liftServoPin = 9;
const int closeServoPin = 10;

const int ledPin = 11;
const int colourPin = 12;
const int irPin = A3;

// Motor configuration
int minPower = 70; int maxPower = 255;
float pGain = 1; float pTerm;
float dGain = 0.25; float dTerm;
float leftPowerTarget; int leftPowerSet;
float rightPowerTarget; int rightPowerSet;
#define FORW = bool true;
#define BACK = bool false;
#define LEFT = int 2;
#define RIGHT = int 3;

// Key geometry
const int slots = 120; // Code wheel slots per revolution
const float wheelDia = 104; // Wheel diameter, mm
const float wheelSep = 244; // Separation between wheel mid-thicknesses, mm
const float pi = 3.141593;
const float wheelTravel = pi*wheelDia;

// Timing configuration
int loopTimeLast;
int intRate = 10000; // Sensor interrupt timing rate, Hz

// Telemetry configuration
String stateVector;
int updateRate  = 4;
int stateID = 0;
int stage = 0;

IDPLib IDP; // Create instance of IDPLib class

// Setup, initialise, check sensors, setup network, everything that should only run once
void setup() {
    Serial.begin(9600);
    delay(100);
    run();
    IDP.debugStart("TheBigBlockCarriers", "IDPL209BBC");

    IDP.lineStart(leftLinePin, rightLinePin);
    IDP.encoderStart(leftEncoderPin, rightEncoderPin);
    IDP.irStart(irPin);
    IDP.colourStart(colourPin);

    pinMode(ledPin, OUTPUT);
}
void loop () {
    // Listen for commands from python here
    // send telemetry at regular intervals (measure loop time), if stage > 0

    switch (stage) {     
        case 0:
            // In startup
            int waits = 1;
            Serial.println("CASE 0: PRE-START COMPLETE.");
            while (!IDP.connected) {
                delay(5000);
                Serial.print("AWAITING CONNECTION... (")
                Serial.print(waits);
                Serial.println(")");
                waits++;
            }
            stage = 1;
            break;

        case 1: 
        // Connection received, start receiving telemetry data, wait for go signal
            int waits = 1;
            IDP.send("CASE 1: CLIENT CONNECTED, STARTING TELEMETRY STREAM AT ");
            IDP.send((String) updateRate);
            IDP.send(" Hz.");
            while (!IDP.startFlag) {
                if (IDP.newOut && IDP.out == "some command from python to start") {

                }
                delay(5000);
                IDP.send("AWAITING START COMMAND... (");
                IDP.send((String) waits);
                IDP.send(")");
                waits++;
            }
            stage = 2;
            break;

        case 2: 
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

        case 3: 
            // Drive a hardcoded distance to the search point using encoders and line following
            // consider using encoders and line sensors to try and precisely align on the centre of the line?
            // case = 4

        case 4: 
            // Enter search mode
            // Sweep out an arc by rotating some fixed number of degrees on the spot, reading the IR sensor, repeating
            // When the IR sensor reading changes dramatically
                //perform a refind scan around the block
                //if the measured distance and angle through which the object are detected are consistent with the expected block size
                    // case = 5

        case 5: 
            // Determine required rotation and distance to block
            // Perform the rotation, then drive forwards to expected 15 cm distance.
            // Reacquire block
            // if reacquire fails
                // Reverse the movements just performed, reacquire line and go again from case 4

            // else perform fine alignment with colour sensor
        case 6: 
            // Perform colour detection, store the result for later so we know where we need to go
            // Fine alignment with grabber

        case 7: 
            // Activate block retrieval mechanism
            // if block retrieval fails:
                // Backtrack all movements to case 4, scan again

        case 8: 
            // Backtrack to line
            // need to modify case 2 code to be reusable at this point - follow line to delivery area

        case 9: 
            // using knowledge of block colour, any previously retrieved blocks, navigate to correct drop area

        case 10: 
            // Align with target, deliver block 

        case 11: 
            // Return to line, repeat cases 4 to 11 until all 4 blocks done

        case 12: 
            // Return to homebase, potentially implement timer such that robot knows when time is running out and will return to homebase, aborting remaining blocks
    }
}