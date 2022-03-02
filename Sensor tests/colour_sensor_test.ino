const int pin = ;//insert pin number

void setup() {
    pinMode(pin, INPUT);
    Serial.begin(9600);
}

void loop() {
    if (digitalRead(pin) == HIGH) {
        Serial.println("Detected red");
        delay (500);
    }

    else if (digitalRead(pin) == LOW) {
        Serial.println("Detected blue");
        delay (500);
    }
    else {
        Serial.println("No reading");
    }

}

