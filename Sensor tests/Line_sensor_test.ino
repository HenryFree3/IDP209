int i;

void setup() {
    pinMode(3, INPUT);
    Serial.begin(9600);
}

void loop() {
    if (digitalRead(3) == LOW) {
        Serial.println("Detected LOW");
        delay(250);
    }
    else if (digitalRead(3) == HIGH) {
        Serial.println("Detected HIGH");
        delay(250);
    }
    else {
        Serial.println("Read failed");
    }
}