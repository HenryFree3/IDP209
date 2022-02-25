// pulse generation of 10 microseconds
// 60 ms between pulses

int echo = 4; // set output pin
int trigger = 3; // set input pin

int duration = 0;
int inPin_stat = 0;

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

}

void pulsegeneration() {  
    Serial.println("Sending pulse");
    digitalWrite(trigger, HIGH);
    Serial.println("pulse high");
    Serial.println(digitalRead(trigger));
    delayMicroseconds(15);
    digitalWrite(trigger, LOW);
    Serial.println("pulse low");
    Serial.println(digitalRead(trigger));
    delay(60);

}
