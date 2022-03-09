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

class IDPLib {
    public:
        IDPLib(String ssid="TheBigBlockCarriers", String password="IDPL209BBC", int port=23, int baudRate=9600);
        void debugStart(String ssid, String password);
        void motorStart(bool strictCheck=true);
        void lineStart(int sensePinLeft, int sensePinRight);
        void irStart(int irPin);
        void encoderStart(int encoderPinLeft, int encoderPinRight, float pollRate = 50);
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

        bool connected;
        bool startFlag = false;
        bool skip;
        bool newOut;
        int APStatus;
        char out[32];

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