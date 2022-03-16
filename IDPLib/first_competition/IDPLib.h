/* 
    IDP L209 library of common functions.
        - Host a local WiFi network using WiFiNINA module
        - Set up PC -> Arduino remote communication over Telnet
        - Set up Adafruit Motor Shield
        - Functions for remote debugging
*/

#ifndef IDPLib_h
#define IDPLib_h

#include "Arduino.h"
#include "SPI.h"
#include "WiFiNINA.h"
#include "Adafruit_MotorShield.h"
#include "Servo.h"

class IDPLib {
    public:
        IDPLib(String ssid="TheBigBlockCarriers", String password="IDPL209BBC", int port1=23, int port2=22, int baudRate=9600);
        void debugStart(String ssid, String password);
        void motorStart(bool strictCheck=true);
        void lineStart(int sensePinLeftLine, int sensePinRightLine);
        void encoderStart(int encoderPinLeft, int encoderPinRight, float pollRate = 50);
        void telemStart();
        void telemUpdate();
        void liftRaise(int liftAng1, int liftAng2);
        void liftLower(int liftAng1, int liftAng2);
        void grabberOpen(int grabAng1, int grabAng2);
        void grabberClose(int grabAng1, int grabAng2);
        void colourStart(int colourSensePin);
        void irStart(int irSensePin);
        float irRead();
        int colourRead();

        void send(String message);
        void refresh();
        void wifiStatus();

        int lineRead();
        
        void goStraight(float dist);

        bool test;

        // Servo objects
        Servo grabServo;
        Servo liftServo;

        // Telemetry variables
        const int updateRate  = 4;
        WiFiClient telemClient;
        bool telemActive;
        bool telemConnected = false;
        int telemPort;
        bool newCommand;
        // State vector to send
        String telemPre = "START:";
        int stateID = 0; // Identifier for the particular state vector.
        uint32_t lastTelemTime = 0; // Micros when last update was posted
        uint32_t lastPeriod; // Millis between current and last update
        int lastLeftCounts; // Lifetime counts of left encoder on last update
        int lastRightCounts; // Lifetime counts of right encoder on last update
        float rotation; // Rotation in degrees since last update
        String telemPost = ":END";
        
        // Key geometry parameters
        const float wheelDia = 104;
        const float wheelSep = 244;
        const int slots = 120;
        //const float 

        // Motor variables
        int minpower = 70; int maxPower = 255;
        float pGain = 1; float pTerm;
        float dGain = 0.25; float dTerm;
        float leftPowerTarget; int leftPowerSet;
        float rightPowerTarget; int rightPowerSet;
        
        bool connected = false;
        bool startFlag = false;
        bool skip;
        bool newOut;
        int APStatus;
        char out[32];
        

        // Pin declarations
        int sensePinLeft;
        int sensePinRight;
        int irPin;
        int colourPin;

        enum lineStates {
            NEITHER = 0, // Neither
            RIGHT = 1, // Right
            LEFT = 2 , // Left
            BOTH = 3, // Both
            ERR = 4 // Error
        };

        enum colours {
            BLUE = 0,
            RED = 1
        };

        WiFiServer server;
        WiFiServer telemServer;
        WiFiClient client;
        Adafruit_MotorShield shield;
        Adafruit_DCMotor motors[4];

    private:
};

class PID {
    public:
        PID();
        float kp;
        float ki;
        float kd;

};

#endif