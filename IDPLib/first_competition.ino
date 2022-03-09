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
            Serial.print("CASE 1: CLIENT CONNECTED, STARTING TELEMETRY STREAM AT ");
            Serial.print(updateRate);
            Serial.println(" Hz.");
            while (!IDP.startFlag) {
                delay(5000);
                Serial.print("AWAITING START COMMAND... (");
                Serial.print(waits);
                Serial.println(")");
                waits++;
            }
            break;

        case 2: 
        // Go signal received, drive as straight as possible and count double lines crossed.
            // Check both line sensors are starting low.
            // Reset wheel encoder variables to zero. (or just take account of them?)

        // Go forward, line follow, PID to minimise error between 2 rotary encoders, count double lines
        case 3: 

        // Double line count reaches 3, slow down, activate search after a hardcoded distance
        case 4: 

        // Search mode
        case 5: 

        // Block found, decide how to get to block (might be able to combine cases 6 and 7)
        case 6: 

        // Get as close as possible to block using line following, approach using dead reckoning
        case 7: 

        // Block classification
        case 8: 

        // Activate block retrieval mechanism, check line sensors; if it fails, backtrack to case 5
        case 9: 

        // Return to line using same route, retrieve data from case 7 to decide on delivery area
        case 10: 

        // Go to delivery area
        case 11: 

        // Align with target, deliver block
        case 12:  

        // Return to line, repeat cases 4 to 11 until all 4 blocks done
        case 13: 

        // Return to homebase, potentially implement timer such that robot knows when time is running out and will return to homebase, aborting remaining blocks
        case 14: 
    }
}