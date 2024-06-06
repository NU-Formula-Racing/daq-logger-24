// NFR24 DAQ Logger Firmware by Ryan Beam
// Written for dual ESP32 logger board by Zach Yu

// included libraries:
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <virtualTimer.h>
#include "esp_can.h"
#include "RTClib.h"

// DAQ Serialization Library Initializations
#include <daqser.hpp>
#include <daqser_can.hpp>

#define VERSION_ARGS(major, minor, patch) major, minor, patch

#define SCHEMA_NAME "esp32dev_test"
#define SCHEMA_VERSION VERSION_ARGS(1, 0, 0)

// The tx and rx pins are constructor arguments to ESPCan, which default to TX = 5, RX = 4
ESPCAN can_bus{};

// defined constants (pins from Zach's NFR23 board):
#define CS 5
#define BAUD_1M 1000000
// timing intervals to collect and hard-save data:

RTC_PCF8523 rtc;

// function declarations here:
fs::File init_SD_card();
fs::File open_SD_card();
void read_write_CAN_data();
void save_SD_card();

// Timer group setup in global scope
VirtualTimerGroup logger_timers; // executing data functions on timer

// Setup the SD card file
fs::File file;
std::string filename;

// setup function for logger:
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    Serial.println("Connecting Serial..."); // wait for serial port to connect. Needed for native USB port only
  }
  file = init_SD_card();
  if (!file) {
    Serial.println("Error opening file.");
  }

  while(!rtc.begin()) {;}
  rtc.start();
  DateTime now = rtc.now();
  std::string timestamp = String(now.year()).c_str();
  timestamp = timestamp + "_" + String(now.month()).c_str() + "_";
  timestamp = timestamp + String(now.day()).c_str() + "_";
  timestamp = timestamp + String(now.hour()).c_str() + "_";
  timestamp = timestamp + String(now.minute()).c_str() + "_";
  timestamp = timestamp + String(now.second()).c_str();
  filename = "/" + timestamp;
  filename = filename + ".txt";
  Serial.printf("FILENAME: %s\n", filename);

  daqser::initialize();
  // Tell daqser what schema we are using to serialize the data
  daqser::setSchema(SCHEMA_NAME, SCHEMA_VERSION);
  daqser::g_canBus.Initialize(ICAN::BaudRate::kBaud1M);
  // Tell daqser to record/send data from these boards
  
  daqser::initializeCAN();

  logger_timers.AddTimer(200U, read_write_CAN_data);
  // logger_timers.AddTimer(5000U, save_SD_card);

  Serial.println("Setup complete.");
}

// logger looping code (just Tick since timers and functions already established):
void loop() {
  logger_timers.Tick(millis());
  daqser::tickCAN();
}

// function definitions used:
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
  file = SD.open(filename.c_str(), "a");
  return file;
}

void read_write_CAN_data() {
  fs::File file = open_SD_card();
  DateTime now = rtc.now();
  file.printf("%s,", now.timestamp());
  daqser::updateSignals();
  std::vector<std::uint8_t> byteData = daqser::serializeFrame();
  // send the data over lora or something
  // send(byteData);
  // print out the bytes
  std::cout << "Data(length:" << byteData.size() << "): ";
  for (std::uint8_t byte : byteData)
  {
    file.print(byte);
  }
  file.println();
}

void save_SD_card() {
  // close SD card to save files
  file.close();
  return;
}