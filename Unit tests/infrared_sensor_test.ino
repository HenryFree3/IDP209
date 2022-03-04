// IR sensor test

const int pin = A0;
int voltage;

void setup () {

    Serial.begin(9600);

    pinMode(pin, INPUT);

    Serial.println("Setup complete");


}

void loop () {

    voltage = analogRead(pin);
    Serial.println(voltage);
    delay(1000);

}
