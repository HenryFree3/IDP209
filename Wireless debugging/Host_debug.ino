/*
  WiFi acess point based debugging.
  Based on Tom Igoe, Nov 2012, adapted by Adafruit.

  Hosts a secure wifi access point using the on-board chipset.
  You can connect to this access point (192.168.4.1) with
  telnet to pass data to the Arduino.

 */

#include <SPI.h>
#include <WiFiNINA.h>
#include <Adafruit_MotorShield.h>

// WIFI SETUP
char ssid[] = "TheBigBlockCarriers"; // your network SSID (name)
char pass[] = "IDPL209BBC";          // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(23);

//SHIELD SETUP
// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Configure all 4 motors outputs. M3 is front left, M4 is front right.
Adafruit_DCMotor *M1 = AFMS.getMotor(1);
Adafruit_DCMotor *M2 = AFMS.getMotor(2);
Adafruit_DCMotor *M3 = AFMS.getMotor(3);
Adafruit_DCMotor *M4 = AFMS.getMotor(4);
Adafruit_DCMotor Motors[] = {*M1, *M2, *M3, *M4};
int step = 500; // Milliseconds between inputs

void setup()
{
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }
  Serial.println("Motor Shield found, proceeding to acess point setup.");

  Serial.println("Begin access point setup.");
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING)
  {
    Serial.println("Creating access point failed.");
    // don't continue
    while (true)
      ;
  }

  // wait 10 seconds for connection:
  delay(10000);

  // start the web server on port 80
  server.begin();

  // you're connected now, so print out the status
  printWiFiStatus();
}

void loop()
{
  // compare the previous status to the current status
  if (status != WiFi.status())
  {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED)
    {
      // a device has connected to the AP
      Serial.println("Device connected to access point.");
      server.write("Client connected.");
    }
    else
    {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from acess point.");
      server.write("Client disconnected.");
    }
  }

  WiFiClient client = server.available();
  // listen for incoming clients
  if (client.available() > 0)
  {
    // read the bytes incoming from the client:
    char thisChar = client.readString()[0];
    // echo the bytes back to the client:
    //server.write(thisChar);
    // echo the bytes to the server as well:
    Serial.println((String) "Received: " + thisChar);

    if (thisChar == 'W')
    {
      // GO FORWARD
      Serial.println("MOVING FORWARD");
      Motors[2].run(FORWARD);
      Motors[3].run(FORWARD);
      Motors[2].setSpeed(255);
      Motors[3].setSpeed(255);
      delay(step);
      Motors[2].setSpeed(0);
      Motors[3].setSpeed(0);
      Motors[2].run(RELEASE);
      Motors[3].run(RELEASE);
    }
    if (thisChar == 'A')
    {
      // TURN LEFT
      Serial.println("TURNING LEFT");
      Motors[2].run(FORWARD);
      Motors[3].run(BACKWARD);
      Motors[2].setSpeed(255);
      Motors[3].setSpeed(255);
      delay(step);
      Motors[2].setSpeed(0);
      Motors[3].setSpeed(0);
      Motors[2].run(RELEASE);
      Motors[3].run(RELEASE);
    }
    if (thisChar == 'S')
    {
      // GO BACK
      Serial.println("MOVING BACK");
      Motors[2].run(BACKWARD);
      Motors[3].run(BACKWARD);
      Motors[2].setSpeed(255);
      Motors[3].setSpeed(255);
      delay(step);
      Motors[2].setSpeed(0);
      Motors[3].setSpeed(0);
      Motors[2].run(RELEASE);
      Motors[3].run(RELEASE);
    }
    if (thisChar == 'D')
    {
      // TURN RIGHT
      Serial.println("TURNING RIGHT");
      Motors[2].run(BACKWARD);
      Motors[3].run(FORWARD);
      Motors[2].setSpeed(255);
      Motors[3].setSpeed(255);
      delay(step);
      Motors[2].setSpeed(0);
      Motors[3].setSpeed(0);
      Motors[2].run(RELEASE);
      Motors[3].run(RELEASE);
    }
  }
}

void printWiFiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}