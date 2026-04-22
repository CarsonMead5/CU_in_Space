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
// - 2 4 Pin LCD Screens (Currently Disabled)
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

// Array of LCD screens (Initialized using the I2C addresses from display.h, assuming standard 16x2 screens)
// LiquidCrystal_I2C LCDs[NUM_LCD] = {
//   LiquidCrystal_I2C(LCD_Address[0], 16, 2),
//   LiquidCrystal_I2C(LCD_Address[1], 16, 2)
// };

// Seven Segment Display (4 Daisy Chained) - Order is (DIN, CLK, CS, numDevices)
LedControl SevSD(SevSD_DIN, SevSD_CLK, SevSD_CS, NUM_SevSD);

// Initializing telemetry packet object
TelemetryPacket telemetry = { 0 };

// Initializing command packet object
CommandPacket command = { 0 };

// Initializing last packet sent variable
long lastPacketSent = millis();
long lastTelemetryTime = millis();
long lastLoRaResetTime = millis();


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
    while (1);
  }

  LoRa.setFrequency(915.0); // Assuming LoRa_Freq is 915.0
  LoRa.setTxPower(19, false);

  // Match settings explicitly
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);

  LoRa.setModeRx();

  // Initializing displays (LCD removed from parameters)
  initDisplays(SevSD);

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
  
  // Wait for the transmission to physically complete before switching modes.
  LoRa.waitPacketSent(); 

  // Switch to receive mode immediately after sending
  LoRa.setModeRx();

  // ---------------- RECEIVE ----------------
  unsigned long start = millis();
  while (millis() - start < 200) {  // 200ms listening window
    if (LoRa.available()) {
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);

      if (LoRa.recv(buf, &len) && len == sizeof(TelemetryPacket)) {
        memcpy(&telemetry, buf, sizeof(TelemetryPacket));
        
        Serial.println("Packet successfully received!");
        lastTelemetryTime = millis();
        // debugReceive(telemetry); // Uncomment if debugReceive is fully implemented
        
        break; // Exit the listening window early since we got our data

        break;  // Exit the listening window early since we got our data
      }
    }
  }

  if (millis() - lastTelemetryTime >= 5000)
  {
    if (millis() - lastLoRaResetTime >= 5000) {
      Serial.println("Telemetry lost: Attempting LoRa Reset...");
      
      // Hardware Reset
      digitalWrite(LoRa_RST, LOW);
      delay(10);
      digitalWrite(LoRa_RST, HIGH);
      
      // FIX 1: Give the radio more time to wake up before calling init()
      delay(50); 

      // FIX 2: Update the timer HERE instead of at bottom, regardless of if init() succeeds or fails
      lastLoRaResetTime = millis(); 

      // Re-initialize and Re-configure
      if (!LoRa.init()) {
        Serial.println("LoRa re-init failed!");
        return; // now safely exit the loop and wait 5 sec to try again
      }

      // Must match your setup() settings exactly
      LoRa.setFrequency(915.0);
      LoRa.setTxPower(19, false);
      LoRa.setSpreadingFactor(7);
      LoRa.setSignalBandwidth(125E3);
      LoRa.setCodingRate4(5);
      LoRa.setModeRx();
      
      Serial.println("LoRa Reset Complete. Listening...");
    }
  }

  // Updating 7SDs (LCD logic is commented out inside display.cpp)
  updateDisplays(telemetry, SevSD);

  // Logging the Data to the OpenLog
  // logTelemetry(telemetry);

  // ---------------- PACING ----------------
  // Pacing combined with the 200ms window means your loop runs roughly 3 times per second
  delay(100);
}