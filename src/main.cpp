// NFR24 DAQ Logger Firmware by Ryan Beam
// Written for dual ESP32 logger board by Zach Yu

// included libraries:
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <virtualTimer.h>
#include <CAN.h>

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
// timing intervals to collect and hard-save data:
#define save_interval 5003
#define data_interval 500

using namespace fs;

// function declarations here:
FILE open_SD_card();
void read_write_CAN_data();
FILE save_SD_card();

// Timer group setup in global scope
VirtualTimerGroup data_timers; // executing data functions on timer
data_timers.AddTimer(save_interval, save_SD_card);
data_timers.AddTimer(data_interval, read_write_CAN_data);

// setup function for logger:
void setup() {
  int n = 0; // number of card saves, global
  Serial.begin(9600);
  FILE file = open_SD_card();
  // if (!file) {
  //   Serial.println("Error opening file.");
  // }
  Serial.println("Setup complete.");
}

// logger looping code (just Tick since timers and functions already established):
void loop() {
  data_timers.Tick(millis());
}

// // function definitions used:
FILE open_SD_card() {
  if (SD.begin(SD_CS)) {
    Serial.println("SD Card detected successfully.");
  } else {
    Serial.println("SD Card not detected.");
  }
  // creating/opening a file on the SD card
  FILE file = SD.open("logger.txt", FILE_WRITE);
  return file
}

void read_write_CAN_data() {
  // in progress, currently testing
  file.println("Hello logger!");
}

FILE save_SD_card() {
  // close SD card to save files
  int n = n + 1;
  SD.close();
  // reopen file to keep collecting data
  file = open_SD_card();
  return file
}