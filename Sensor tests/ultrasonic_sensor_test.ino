// pulse generation of 10 microseconds
// 60 ms between pulses

int echo = 8; // set output pin
int trigger = 7; // set input pin

unsigned long duration = 0;
unsigned long timeout = 2500000;
int val;

void setup() {
    Serial.begin(9600);

    pinMode(trigger, OUTPUT);
    pinMode(echo, INPUT);

}

void loop() {

    Serial.println("Alive");
    pulsegeneration();
    duration = pulseIn(echo, HIGH);
    Serial.println("Pulse duration is");
    Serial.println(duration);
    delay(60);

}

void pulsegeneration() {    
  
    Serial.println("Sending pulse");
    digitalWrite(trigger, LOW);
    digitalWrite(trigger, HIGH);
    Serial.println("pulse high");
    Serial.println(digitalRead(trigger));
    delayMicroseconds(15);
    digitalWrite(trigger, LOW);
    Serial.println("pulse low");
    Serial.println(digitalRead(trigger));

}

void pintesting() {
  
    digitalWrite(echo, HIGH);
    val = digitalRead(echo);
    Serial.println(val);
    delay(1000);
}
