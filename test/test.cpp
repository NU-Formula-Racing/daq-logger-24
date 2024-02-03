// Testing code that may or may not work

// included libraries:
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <virtualTimer.h>
#include <CAN.h>

// Testing
void setup() {
    Serial.begin(9600);
    Serial.println("Hello Worldwide");
}

void loop() {
    Serial.println("looping");
}
