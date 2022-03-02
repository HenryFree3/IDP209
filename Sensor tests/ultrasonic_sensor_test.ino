/* 
    Pulse generation of 10 microseconds
    60 ms between pulses 
*/

const int echoPin = 8; // set input pin
const int triggerPin = 7; // set output pin

unsigned long duration = 0;
unsigned long timeout = 2500000;
int val;
int dist;
int distance;

void setup() {
    Serial.begin(9600);

    pinMode(trigger, OUTPUT);
    pinMode(echo, INPUT);

}

void loop() {

    Serial.println("Alive");
    pulsegeneration();
    duration = pulseIn(echoPin, HIGH);
    distance = distancecalculation();
    Serial.println("Pulse duration is");
    Serial.println(duration);
    Serial.println("Distance in cm is");
    Serial.println(distance);

}

void pulsegeneration() {    
  
    Serial.println("Sending pulse");
    delay(60);
    digitalWrite(triggerPin, LOW);
    digitalWrite(triggerPin, HIGH);
    //Serial.println("pulse high");
    //Serial.println(digitalRead(trigger));
    delayMicroseconds(15);
    digitalWrite(triggerPin, LOW);
    //Serial.println("pulse low");
    //Serial.println(digitalRead(trigger));

}

void pintesting() {
  
    digitalWrite(echoPin, HIGH);
    val = digitalRead(echoPin);
    Serial.println(val);
    delay(1000);

}

void distancecalculation() {
    
    // returns distance in cm
    
    dist = duration / 58

}
