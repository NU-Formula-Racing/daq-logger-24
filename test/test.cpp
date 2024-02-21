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

// CAN stuff
MakeSignedCANSignal(float, 0, 16, 0.01, 0) float_rx_signal{};
MakeUnsignedCANSignal(uint8_t, 16, 8, 1, 0) uint8_t_rx_signal{};
MakeUnsignedCANSignal(bool, 24, 1, 1, 0) bool_rx_signal{};
MakeUnsignedCANSignal(uint32_t, 32, 32, 1, 0) millis_rx_signal{};

CANRXMessage<4> rx_message{can_bus,
                           0x200,
                           []() { Serial.println(rx_message.GetLastReceiveTime()); },
                           float_rx_signal,
                           uint8_t_rx_signal,
                           bool_rx_signal,
                           millis_rx_signal};


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