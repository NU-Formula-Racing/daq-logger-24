// Testing code that may or may not work

// included libraries:
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <virtualTimer.h>
#include <esp_can.h>
ESPCAN can_bus{};

// Test variables
void test_func();
VirtualTimer timer(500, test_func, VirtualTimer::Type::kRepeating);
VirtualTimerGroup group;

MakeSignedCANSignal(float, 10, sizeof(float)*8, 0.1, 0) tsignal{};

// Testing
void setup() {
    Serial.begin(9600);
    timer.Start(millis());
    group.AddTimer(timer);
}

void loop() {
    group.Tick(millis());
}

void test_func() {
    Serial.println("Timer stuff.\n");
}