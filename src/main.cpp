// NFR24 DAQ Logger Firmware by Ryan Beam
// Written for dual ESP32 logger board by Zach Yu

// included libraries:
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <virtualTimer.h>
#include "esp_can.h"

// The tx and rx pins are constructor arguments to ESPCan, which default to TX = 5, RX = 4
ESPCAN can_bus{};

// defined constants (pins from Zach's NFR23 board):
#define CAN_TX 34
#define CAN_RX 27
#define CAN_SHDN 33
#define CAN_RS 26
#define SD_IN 23
#define SD_OUT 19
#define SD_CL 18
#define SD_CS 5
#define SD_CD 34
#define RTC_SDA 21
#define RTC_SCL 22
#define BAUD_1M 1000000
// timing intervals to collect and hard-save data:
#define save_interval 5003
#define data_interval 500

using namespace fs;

// function declarations here:
fs::File open_SD_card();
void read_write_CAN_data(fs::File file);
fs::File save_SD_card(fs::File file);

// Timer group setup in global scope
float save_start = 0;
float read_start = 0;
// VirtualTimerGroup data_timers; // executing data functions on timer
// VirtualTimerGroup data_timers.AddTimer(save_interval, save_SD_card, VirtualTimer::Type::kRepeating);
// VirtualTimerGroup data_timers.AddTimer(data_interval, read_write_CAN_data, VirtualTimer::Type::kRepeating);

// Setup the SD card file
fs::File file;

// setup function for logger:
void setup() {
  int n = 0; // number of card saves, global
  Serial.begin(9600);
  file = open_SD_card();
  if (!file) {
    Serial.println("Error opening file.");
  }
  Serial.println("Setup complete.");
}

// logger looping code (just Tick since timers and functions already established):
void loop() {
  float save_time = millis() - save_start;
  float read_time = millis() - read_start;

// PDR Notes
// - millis counting limit
//

  //data_timers.Tick(millis());
  if (save_time > save_interval) {
    file = save_SD_card(file);
    save_start = millis();
    save_time = 0;
  }
  if (read_time > data_interval) {
    read_write_CAN_data(file);
  }
}

// // function definitions used:
fs::File open_SD_card() {
  if (SD.begin(SD_CS)) {
    Serial.println("SD Card detected successfully.");
  } else {
    Serial.println("SD Card not detected.");
  }
  // creating/opening a file on the SD card
  file = SD.open("logger.txt", FILE_WRITE);
  return file;
}

void read_write_CAN_data(fs::File file) {
  // in progress, currently testing
  file.println("Hello logger!");
}

fs::File save_SD_card(fs::File file) {
  // close SD card to save files
  int n = n + 1;
  file.close();
  // reopen file to keep collecting data
  file = open_SD_card();
  return file;
}