/* 
    Line recovery test, using two sensors.
    Includes WiFi debugging.
*/

#include <SPI.h>
#include <WiFiNINA.h>
#include <Adafruit_MotorShield.h>

const int leftPin = 3;
const int rightPin = 4;
bool leftDetect;
bool rightDetect;

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

//SENSOR CONFIGURATION
float turnAxisDist = 65; // Perpendicular dist from front line sensors to rotation axis, forward positive, mm

void setup() {
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
  Serial.println("Motor Shield found, proceeding to access point setup.");

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
    // Begin WiFi server
    server.begin();
    printWiFiStatus();

    // Set sensor pins for interrupts
    pinMode(leftPin, INPUT_PULLUP);
    pinMode(rightPin, INPUT_PULLUP);

    Motors[2].setSpeed(128);
    Motors[3].setSpeed(128);
}

void loop() {
  leftDetect = digitalRead(leftPin);
  rightDetect = digitalRead(rightPin);
  // compare the previous status to the current status
  delay(1);
  //Serial.println("Alive");
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
      Serial.println("Device disconnected from access point.");
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
    server.write(thisChar);
    // echo the bytes to the server as well:
    Serial.println((String) "Received: " + thisChar);
 }

  if (leftDetect == LOW && rightDetect == LOW) {
    Log("Both sensors are low. Go straight");
    Motors[2].run(FORWARD);
    Motors[3].run(FORWARD);
    delay(100);
    Motors[2].run(RELEASE);
    Motors[3].run(RELEASE);
  }
  else if ((leftDetect == HIGH && rightDetect == LOW) or (leftDetect == HIGH && rightDetect == HIGH)) {
     if (rightDetect == LOW) {
        Log("Left is high, right is low. TURN LEFT.");
     }
    else {
      Log("Left is high, right is high. TURN LEFT.");
    }
    Motors[2].run(FORWARD);
    Motors[3].run(BACKWARD);
    delay(100);
    Motors[2].run(RELEASE);
    Motors[3].run(RELEASE);
  }
  else if (leftDetect == LOW && rightDetect == HIGH) {
    Log("Left is low, right is high. TURN RIGHT.");
    Motors[2].run(BACKWARD);
    Motors[3].run(FORWARD);
    delay(100);
    Motors[2].run(RELEASE);
    Motors[3].run(RELEASE);
  }
}

void Log(const char message[]) {
  char messageArr[100]; // To hold message
  strcpy(messageArr, message);
  strcat(messageArr, "\r\n");
  Serial.println(message);
  server.write(messageArr);
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