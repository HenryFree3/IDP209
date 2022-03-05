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
        void encoderStart(int encoderPinLeft, int encoderPinRight, float pollRate = 50);

        void send(String message);
        void refresh();
        void wifiStatus();

        int lineRead();
        void encoderHandler();

        bool test;

        bool connected;
        bool skip;
        bool newOut;
        int APStatus;
        char out[32];

        int sensePinLeft;
        int sensePinRight;

        enum lineStates {
            NEITHER = 0, // Neither
            RIGHT = 1, // Right
            LEFT = 2 , // Left
            BOTH = 3, // Both
            ERR = 4 // Error
        };

        WiFiServer server;
        WiFiClient client;
        Adafruit_MotorShield shield;
        Adafruit_DCMotor motors[4];

    private:
};

#endif