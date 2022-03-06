#include "Arduino.h"
#include "SPI.h"
#include "WiFiNINA.h"
#include "Adafruit_MotorShield.h"
#include "IDPLib.h"

IDPLib::IDPLib(String ssid, String password, int port, int baudRate) : server(port) {
    // Constructor for IDPLib class
    Serial.begin(baudRate); // Ensure serial is running
    APStatus = WL_IDLE_STATUS; // AP status prior to start
    WiFiServer server(23); // Use default telnet port
    connected = false;  
}


void IDPLib::debugStart(String ssid, String password) {
    // Start remote WiFi capability
    delay(50);

    Serial.println("**************************");
    Serial.println("*** START REMOTE DEBUG ***");
    Serial.println("**************************\n");

    Serial.println("Begin access point setup.\n");
    // Check for WiFi module
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed! Fix & reset.\n");
        while (true) {
            delay(10); // Hang
        }
    }

    APStatus = WiFi.beginAP(ssid.c_str(), password.c_str());
    // Check AP is running
    if (APStatus != WL_AP_LISTENING) {
        Serial.println("Creating access point failed. Fix & reset.\n");
        while (true) {
            delay(10); // Hang
        }
    }
    
    // Begin WiFi server
    server.begin();
    wifiStatus();

    send("**************************");
    send("*** REMOTE DEBUG READY ***");
    send("**************************\n");
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
    Serial.println(message);
    // Just serial print if no server is available
    //if (server.available()) {
    if (connected) {
        //Serial.println("Message length of : " + String(message.length()));
        char messageArr[message.length()]; // Char limit based on message size
        //strcpy(messageArr, message.c_str());
        //strcat(messageArr, String("\r\n")); // Append newline for print
        server.print(message);
  }
}

void IDPLib::refresh() {
    // Checks on access point and client, receives new data from client

    if (APStatus != WiFi.status()) {
        APStatus = WiFi.status(); // Update if stale
        Serial.print("New AP status code: ");
        Serial.println(APStatus);
    }

    // Only check for a client if there is not one connected already
    if (connected == false) {
        if (server.available()) {
            connected = true;
            skip = true; // Flag to skip reading first bytes
            client = server.available();
            client.flush();
            send("New client connection");
        }
    }
    else if (client.status() == 0) {
        // If client connection is no longer "established":
        send("Client lost");
        client.stop();
        connected = false;
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
    // Get detauks of hosted access point
    send("SSID: ");
    send(WiFi.SSID());
    IPAddress ip = WiFi.localIP();
    // Type conversion
    String ip_str = String(ip[0]) + String(".") + 
                    String(ip[1]) + String(".") + 
                    String(ip[2]) + String(".") + 
                    String(ip[3]);
    char ip_char[16];
    ip_str.toCharArray(ip_char, 16);
    send("IP Address: ");
    send(ip_char);
    send("");
}


void IDPLib::lineStart(int sensePinLeft, int sensePinRight) {
    // Simple set up - consider deleting this function
    pinMode(sensePinLeft, INPUT_PULLUP);
    pinMode(sensePinRight, INPUT_PULLUP);
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
    pinMode(encoderPinLeft, INPUT_PULLUP);
    pinMode(encoderPinRight, INPUT_PULLUP);
}

void IDPLib::encoderHandler() {
    test = true;
}