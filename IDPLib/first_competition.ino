/*
    First competition integration
*/

// Use switch...case structure to ensure algorithm flow


// Setup, initialise, check sensors, setup network, everything that should only run once
void setup() {


    // Line sensors for line following
    const int leftLinePin = ;
    pinMode(leftLinePin, INPUT_PULLUP);

    const int rightLinePin = ;
    pinMode(rightLinePin, INPUT_PULLUP);

    // Line sensors for target alignment (?)
    const int leftTargetAlignmentPin = ;
    pinMode(leftTargetAlignmentPin, INPUT_PULLUP);

    const int rightTargetAlignmentPin = ;
    pinMode(rightTargetAlignmentPin, INPUT_PULLUP);

    // Infrared sensor
    const int infraredPin = ;
    pinMode(infraredPin, INPUT_PULLUP);

    // Colour detection sensor
    const int colorPin = ;
    pinMode(colorPin, INPUT_PULLUP);

    // Flashing LED as per spec
    const int flashingLEDPin = ;
    pinMode(flashingLEDPin, INPUT_PULLUP);

    // Rotary encoders
    const int leftEncoderPin = ;
    pinMdooe(leftEncoderPin, INPUT_PULLUP);

    const int rightEncoderPin = ;
    pinMode(rightEncoderPin, INPUT_PULLUP);

    // Variables setup

    // 0 is red, 1 is blue
    int color; 

    // Network setup


}
void loop () {

}

void run() {

    switch (stage) {     
        
        // Connection received, start receiving telemetry data, wait for go signal
        case 0: 

        // Go signal received
        case 1: 

        // Go forward, line follow, PID to minimise error between 2 rotary encoders, count double lines
        case 2: 

        // Double line count reaches 3, slow down, activate search after a hardcoded distance
        case 3: 

        // Search mode
        case 4: 

        // Block found, decide how to get to block (might be able to combine cases 5 and 6)
        case 5: 
        
        // Get as close as possible to block using line following, approach using dead reckoning
        case 6: 

        // Block classification
        case 7: 

            // Blue
            if (digitalRead(colorPin) == HIGH) {
                color = 1;
                Serial.println("Detected blue");
            }

            else if (digitalRead(colorPin) == LOW) {
                color = 0;
                Serial.println("Detected red");
            }




        // Activate block retrieval mechanism, check line sensors; if it fails, backtrack to case 4
        case 8: 

        // Return to line using same route, retrieve data from case 7 to decide on delivery area
        case 9: 

        // Go to delivery area
        case 10: 

        // Align with target, deliver block
        case 11:  

        // Return to line, repeat cases 4 to 11 until all 4 blocks done
        case 12: 

        // Return to homebase, potentially implement timer such that robot knows when time is running out and will return to homebase, aborting remaining blocks
        case 13: 
    }
}