// NFR24 DAQ Logger Firmware by Ryan Beam
// Written for dual ESP32 logger board by Zach Yu

// included libraries:
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <virtualTimer.h>
#include "esp_can.h"
#include "daqser.h"
#include "RTClib.h"

// The tx and rx pins are constructor arguments to ESPCan, which default to TX = 5, RX = 4
ESPCAN can_bus{};

// defined constants (pins from Zach's NFR23 board):
#define CS 5
#define BAUD_1M 1000000
// timing intervals to collect and hard-save data:
#define save_interval 5003
#define data_interval 500

// using namespace fs;
RTC_PCF8523 rtc;

// function declarations here:
fs::File init_SD_card();
fs::File open_SD_card();
void read_write_CAN_data(fs::File file, unsigned long time);
fs::File save_SD_card(fs::File file);

// Timer group setup in global scope
float save_start = 0;
float read_start = 0;
VirtualTimerGroup data_timers; // executing data functions on timer
VirtualTimerGroup save_SD;

// Setup the SD card file
fs::File file;

// setup function for logger:
void setup() {
  int n = 0; // number of card saves, global
  Serial.begin(96000);
  while (!Serial) {
    Serial.println("Connecting Serial..."); // wait for serial port to connect. Needed for native USB port only
  }
  file = init_SD_card();
  if (!file) {
    Serial.println("Error opening file.");
  }
  daqser::initialize();
  daqser::setSchema(ask evan);

  rtc.begin();
  rtc.start();

  data_timers.AddTimer(100U, read_write_CAN_data);
  save_SD.AddTimer(5000U, save_SD_card);

  Serial.println("Setup complete.");
}

// logger looping code (just Tick since timers and functions already established):
void loop() {
  data_timers.Tick();
  save_SD.Tick();
}

// // function definitions used:
fs::File init_SD_card() {
  SPI.begin(18, 19, 23, CS);
  SPI.setDataMode(SPI_MODE0);
  pinMode(CS, OUTPUT);
  delay(100);
  while (!SD.begin(CS, SPI)) {
    Serial.println("SD Card NOT detected successfully.");
  }
  Serial.println("SD card detected successfully.");
  // creating/opening a file on the SD card
  file = SD.open("/data.txt", "w");
  return file;
}

fs::File open_SD_card() {
  file = SD.open("/data.txt", "a");
  return file;
}

void read_write_CAN_data(fs::File file, unsigned long time) {
  daqser::updateSignals(boards);
  vector<uint8_t> data = daqser::serializeFrame();
  now = rtc.now();
  file.printf("%s,", now.timestamp());
  for (auto & d : data) {
    file.printf("%d," d);
  }
  file.printf("\n");
}

void save_SD_card(fs::File file) {
  // close SD card to save files
  file.close();
  return;
}