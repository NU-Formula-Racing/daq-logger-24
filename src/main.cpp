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
  // if (!file) {
  //   Serial.println("Error opening file.");
  // }
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

  // FOLLOWING CODE TEMPLATE FROM Chris Uustal's CAN DOCUMENT
  // TO BE STUDIED AND ADAPTED

  // #include <FlexCAN_T4.h> //requires Teensyduino 
  // #define BAUD_1M   1000000

  // //Create CANBus object
  // FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> CANBus1; 

  // //Create CANBus send / receive messages
  // CAN_message_t tx1msg;
  // CAN_message_t rx1msg;

  // //Variables used for timing
  // unsigned long t1ref;
  // unsigned long t1delay = 100;

  // void setup() {
  //   //Initialize the CANBus at a speed of 1Mhz
  //   CANBus1.begin();
  //   CANBus1.setBaudRate(BAUD_1M);

  //   //Populate values to the message being transmitted
  //   tx1msg.id = 0x100;	//Message ID = 0x100
  //   tx1msg.len = 8; 		//Length must match buffer size
  //           //Only values 0-7 are acceptable
  //           //in the standard CAN protocol
  //   //buf (buffer) stores the data of the message
  //   tx1msg.buf[0] = 0x1; 	//any byte value 0x0 to 0xFF
  //   tx1msg.buf[1] = 0x2;
  //   tx1msg.buf[2] = 0x3;
  //   tx1msg.buf[3] = 0x4;
  //   tx1msg.buf[4] = 0x5;
  //   tx1msg.buf[5] = 0x6;
  //   tx1msg.buf[6] = 0x7;
  //   tx1msg.buf[7] = 0x8;

  //   //For timing reference
  //   t1ref = millis();
  // }

  // void loop() {
    
  //   //Timer used for sending tx1msg every 100ms
  //   if(t1ref + t1delay <= millis()){
  //     t1ref = millis();

  //     //Actually send the message
  //     CANBus1.write(tx1msg);
  //   }

  //   //Attempt to read in the newest CAN message 
  //   //If we successfully read a message, print some values from it
  //   if(CANBus1.read(rx1msg)){
      
  //     //Print the message ID
  //     Serial.print("ID: 0x"); Serial.print(rx1msg.id, HEX );
      
  //     //Print the data from the message
  //     Serial.print("\t DATA: ");
  //     for ( uint8_t i = 0; i < rx1msg.len; i++ ) {
  //       Serial.print(rx1msg.buf[i]); Serial.print("");
  //     }

  //     Serial.println("");
  //   }
  // }

  // END OF CODE TEMPLATE

}

fs::File save_SD_card(fs::File file) {
  // close SD card to save files
  int n = n + 1;
  file.close();
  // reopen file to keep collecting data
  file = open_SD_card();
  return file;
}