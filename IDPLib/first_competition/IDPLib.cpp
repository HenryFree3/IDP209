#include "Arduino.h"
#include "SPI.h"
#include "WiFiNINA.h"
#include "Adafruit_MotorShield.h"
#include "Servo.h"
#include "IDPLib.h"

IDPLib::IDPLib(String ssid, String password, int port1, int port2, int baudRate) : server(port1), telemServer(port2) {
    // Constructor for IDPLib class
    telemPort = port2;
    Serial.begin(baudRate); // Ensure serial is running
    APStatus = WL_IDLE_STATUS; // AP status prior to start
    WiFiServer server(23); // Use default telnet port

    grabServo.attach(10); // Adafruit motorshield servo pins
    liftServo.attach(9);
}


void IDPLib::debugStart(String ssid, String password) {
    // Start remote WiFi capability
    send("*** START REMOTE DEBUG ***\n\n");
    send("Begin access point setup.\n");
    // Check for WiFi module
    if (WiFi.status() == WL_NO_MODULE) {
        send("Communication with WiFi module failed! Fix & reset.\n");
        while (true) {
            delay(10); // Hang
        }
    }

    APStatus = WiFi.beginAP(ssid.c_str(), password.c_str());
    // Check AP is running
    if (APStatus != WL_AP_LISTENING) {
        send("Creating access point failed. Fix & reset.\n");
        while (true) {
            delay(10); // Hang
        }
    }
    // Begin WiFi server
    server.begin();
    wifiStatus();
    send("\n*** REMOTE DEBUG READY ***\n\n");
}

void IDPLib::telemStart() {
    // Start up secondary telemetry server
    telemServer.begin();
    telemActive = true;
    send("Telemetry server running on port "+ (String) telemPort + ".\n");
}

void IDPLib::telemUpdate() {
    if (telemActive && telemConnected) {
        // Construct state vector
        stateID++;
        telemServer.print(telemPre + " " + (String) stateID + " " + (String) lastPeriod + " "

        + telemPost);
    }
    else if (!telemConnected) {
        send("A telemetry update was scheduled, but there is no connection to receive it.\n");
    }
    else {
        send("Telemetry update failed: telemetry server not started.\n");
    }
}


void IDPLib::motorStart(bool strictCheck) {
    // Check for and start motor shield
    shield = Adafruit_MotorShield(); // Initialise motorshield object

    // Set pointers for each motor, construct motor array
    motors[0] = *shield.getMotor(1);
    motors[1] = *shield.getMotor(2);
    motors[2] = *shield.getMotor(3);
    motors[3] = *shield.getMotor(4);
    //motors = {*M1, *M2, *M3, *M4};

    if (!shield.begin()) {
        if (strictCheck == true) {
            Serial.println("Could not find Motor Shield. Check connections & reset.\n");
            while (true) {
                delay(10); // Hang
            }
        }
        else {
            // If user has specified to ignore detection failure
            Serial.println("Shield failed to initialise, continuing...");
        }
    }
    else {
        Serial.println("Motor Shield found.");
    }
}

void IDPLib::send(String message) {
    // Send your data to serial (wired) and server (WiFi)
    Serial.print(message);
    // Just serial print if no server is available
    //if (server.available()) {
    if (connected) {
        //Serial.println("Message length of : " + String(message.length()));
        //char messageArr[message.length()]; // Char limit based on message size
        //strcpy(messageArr, message.c_str());
        //strcat(messageArr, String("\r\n")); // Append newline for print
        server.print(message);
  }
}

void IDPLib::refresh() {
    // Checks on access point and clients, receives new data from clients
    if (APStatus != WiFi.status()) {
        APStatus = WiFi.status(); // Update if stale
        send("New AP status code: ");
        send((String) APStatus + "\n");
    }

    // Only check for a client if there is not one connected already
    if (connected == false) {
        if (server.available()) {
            connected = true;
            skip = true; // Flag to skip reading first bytes
            client = server.available();
            client.flush();
            send("New client connection\n");
        }
    }
    else if (client.status() == 0) {
        // If client connection is no longer "established":
        send("Client lost\n");
        client.stop();
        connected = false;
    }

    if (telemConnected == false) {
        if (telemServer.available()) {
            telemConnected = true;
            telemClient = telemServer.available();
            telemServer.write("ACK TELEMETRY CONNECTION"); // Acknowledge the client
            telemClient.flush();
            telemClient.readString();
            send("New telemetry connection\n");
        }
    }
    else if (telemClient.status() == 0 ) {
        send("Telemetry client lost\n");
        telemClient.stop();
        telemConnected = false;
    }

    if (client.available() > 0) {
        // Read new data from client
        newOut = true;
        // newOut flag shows if out has updated since last refresh() call
        if (skip) {
            skip = false;
            client.readString(); // Clear buffer
        }
        else {
            String receiveBufferStr = client.readString();
            char receiveBuffer[32];
            receiveBufferStr.toCharArray(receiveBuffer, 32);
            strcpy(out, receiveBuffer);
        }
    }
    else {
        newOut = false;
    }
}

void IDPLib::wifiStatus() { 
    // Get details of hosted access point
    Serial.print((String) "SSID: ");
    Serial.println((String) WiFi.SSID());
    IPAddress ip = WiFi.localIP();
    // Type conversion
    String ip_str = String(ip[0]) + String(".") + 
                    String(ip[1]) + String(".") + 
                    String(ip[2]) + String(".") + 
                    String(ip[3]);
    char ip_char[16];
    ip_str.toCharArray(ip_char, 16);
    Serial.print("IP Address: ");
    Serial.print(ip_char);
    Serial.println("");
}


void IDPLib::lineStart(int sensePinLeftLine, int sensePinRightLine) {
    // Simple set up - consider deleting this function
    sensePinLeft = sensePinLeftLine;
    sensePinRight = sensePinRightLine;

    pinMode(sensePinLeft, INPUT);
    pinMode(sensePinRight, INPUT);
}

int IDPLib::lineRead() {
    // Poll the line sensors, return state
    // describing orientation with respect to line
    bool leftDetect = digitalRead(sensePinLeft);
    bool rightDetect = digitalRead(sensePinRight);

    if (!leftDetect && !rightDetect) {
        return NEITHER; // Neither sensor high, can go straight
    }
    else if (!leftDetect && rightDetect) {
        return RIGHT; // Right sensor high only, should turn right
    }
    else if (leftDetect && !rightDetect) {
        return LEFT; // Left sensor high only, should turn left
    }
    else if (leftDetect && rightDetect) {
        return BOTH; // Both sensors high, likely crossing a line
    }
    else {
        return ERR; // Error state, invalid read
    }
}

void IDPLib::liftRaise(int liftAng1, int liftAng2) {
    int pos;
    for (pos = liftAng2; pos >= liftAng1; pos -= 1) {
        liftServo.write(pos);
        delay(5);
    }
}

void IDPLib::liftLower(int liftAng1, int liftAng2) {
    int pos;
    for (pos = liftAng1; pos <= liftAng2; pos += 1) {
        liftServo.write(pos);
        delay(5);
    }
}

void IDPLib::grabberOpen(int grabAng1, int grabAng2) {
    int pos;

    for (pos = grabAng2; pos >= grabAng1; pos -= 1) {
        grabServo.write(pos);
        delay(5);
    }
}

void IDPLib::grabberClose(int grabAng1, int grabAng2) {
    int pos;

    for (pos = grabAng1; pos <= grabAng2; pos += 1) {
        grabServo.write(pos);
        delay(5);
    }
}


void IDPLib::colourStart(int colourSensePin) {
    colourPin = colourSensePin;
    pinMode(colourPin, INPUT_PULLUP);
}

void IDPLib::irStart(int irSensePin) {
    irPin = irSensePin;
    pinMode(irPin, INPUT_PULLUP);
}

float IDPLib::irRead() {
    // Take a number of IR measurements, using the average reading return the distance to object.
    const int measurements = 1000;
    float dist;
    int reading;
    uint32_t cumReading;

    for (int count = 0; count < measurements; count++) {
        reading = analogRead(irPin);
        cumReading += reading;
    }
    reading = (cumReading/measurements);
    cumReading = 0;
    return 30554*pow(reading, -0.90943); // Decay relationship, constants found from calibration
}

int IDPLib::colourRead() {
    if (digitalRead(colourPin) == RED) { // Red
        return BLUE;
    }
    else { // Blue
        return BLUE;
    } 
}

void IDPLib::encoderStart(int encoderPinLeft, int encoderPinRight, float pollRate) {
    // Set up the encoders. Must run in setup().
    // pollRate is the encoder polling frequency, recommend 50 Hz.
    // Hardware timer interrupts are preferred to pin change interrupts to mitigate
    // debouncing and simplify velocity calculations.

    TCB0.CTRLB = TCB_CNTMODE_INT_gc; // Compare mode
    TCB0.CCMP = 250000/pollRate; // Tick rate is 250 kHz, so 2500 is 1/100th, frequency of 100 Hz
    TCB0.INTCTRL = TCB_CAPT_bm; // Enable interrupt
    TCB0.CTRLA = TCB_CLKSEL_CLKTCA_gc | TCB_ENABLE_bm; // Use timer A as the clock, enable

    // Set input pins
    pinMode(encoderPinLeft, INPUT);
    pinMode(encoderPinRight, INPUT);
}

void IDPLib::goStraight(float dist) {
    // Drive as straight as possible for a set distance.

}
