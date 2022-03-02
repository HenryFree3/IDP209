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
        void wifiStatus();
        void debugStart(String ssid, String password);
        void motorStart(bool strictCheck=true);
        void send(const char message[]);
        void refresh();

        bool connected;
        bool skip;
        bool newOut;
        int APStatus;
        char out[32];

        WiFiServer server;
        WiFiClient client;
        Adafruit_MotorShield shield;
        Adafruit_DCMotor motors[4];

    private:
};

#endif