// Arduino Mega Code for Ground Side GSE System
// Contributors: Carson Mead
// Date Modified: 3/12/26

// -------------------------------
// Ground Side System Overview:
// -------------------------------
// - 1 Arduino Mega
// - 915 MHz LoRa Transceiver
// - 8 Control Switches
// - 1 Abort Switch
// - 2 Control Buttons
// - 4 MAX7129 7 Segment Displays
// - 2 4 Pin LCD Screens
// - 1 MicroSD Data Logger

// -------------------------------
// Including Needed Header Files
// -------------------------------
#include "packets.h"
#include "config.h"
#include "lora.h"
#include "display.h"
#include "log.h"
#include "switches.h"
#include "crc.h"

// -------------------------------
// Initializing Key Objects and Structures
// -------------------------------

// LoRa transceiver
RH_RF95 LoRa(LoRa_CS, LoRa_INT);
// Array of LCD screens
// LiquidCrystal_I2C LCDs[NUM_LCD] = {}
// Seven Segment Display (4 Daisy Chained)
// LedControl SevSD(SevSD_DIN,SevSD_CS,SevSD_CLK,NUM_SevSD);
// Initializing telemetry packet object
TelemetryPacket telemetry = { 0 };
// Initializing command packet object
CommandPacket command = { 0 };
// Initializing last packet sent variable
long lastPacketSent = millis();


// -------------------------------
// Main Setup
// -------------------------------
void setup() {

  pinMode(LoRa_RST, OUTPUT);
  digitalWrite(LoRa_RST, HIGH);

  Serial.begin(115200);
  delay(1000);

  // Reset radio
  digitalWrite(LoRa_RST, LOW);
  delay(10);
  digitalWrite(LoRa_RST, HIGH);
  delay(10);

  SPI.begin();

  if (!LoRa.init()) {
    Serial.println("LoRa init failed");
    while (1)
      ;
  }

  LoRa.setFrequency(LoRa_Freq);
  LoRa.setTxPower(19, false);

  // Match settings explicitly
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);

  LoRa.setModeRx();

  Serial.println("Ground Ready");

  // Initializing all displays
  // initDisplays();

  // Setting Up Switches
  initSwitches();

  // Printing that System is Ready
  Serial.println("Ground System Ready");
  LoRa.setModeRx();
}


// -------------------------------
// Main Loop
// -------------------------------
void loop() {
  // Reading Switch Commands
  readSwitches(command);

  // ---------------- SEND ----------------
  // Send the command packet to the rocket
  sendCommands(LoRa, command);
  
  // CRITICAL FIX FROM SECTION 2: 
  // Wait for the transmission to physically complete before switching modes.
  // (Note: If waitPacketSent() is already inside your sendCommands() function, 
  // having it here twice won't hurt, but it's essential that it executes).
  LoRa.waitPacketSent(); 

  // Switch to receive mode immediately after sending
  LoRa.setModeRx();

  // ---------------- RECEIVE ----------------
  unsigned long start = millis();
  while (millis() - start < 200) {  // 200ms listening window
    if (LoRa.available()) {
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);

      // Combined check from Section 2 logic
      if (LoRa.recv(buf, &len) && len == sizeof(TelemetryPacket)) {
        memcpy(&telemetry, buf, sizeof(TelemetryPacket));
        
        Serial.println("Packet successfully received!");
        debugReceive(telemetry);
        
        break; // Exit the listening window early since we got our data
      }
    }
  }

  // Updating LCDs and 7SDs
  // updateDisplays(LCDS, SevSD);

  // Logging the Data to the OpenLog
  // logTelemetry(telemetry);

  // ---------------- PACING ----------------
  // Replaces the outer millis() timer from Section 1. 
  // This delay pacing combined with the 200ms window means your loop 
  // runs cleanly roughly 3 times per second, just like Section 2.
  delay(100);
}