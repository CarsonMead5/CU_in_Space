// Arduino Mega Code for Rocket Side GSE System
// Contributors: Carson Mead, Ryan Chen, Nussbaumer
// Date Modified: 3/12/26

// -------------------------------
// Rocket Side System Overview:
// -------------------------------
// - 1 Arduino Mega
// - 915 MHz LoRa Transceiver
// - 2 1600 psi Pressure Transducers
// - 1 1000 psi Pressure Transducer
// - 1 10 kN Load Cell
// - 4 20 kgcm Servo Motor Controlled Valves
// - 1 High Pressure Solenoid Valve


// -------------------------------
// Including Needed Header Files
// -------------------------------
#include "packets.h"
#include "config.h"
#include "sensors.h"
#include "lora.h"
#include "actuators.h"
#include "crc.h"

#include "log.h"

// -------------------------------
// Initializing Key Objects and Structs
// -------------------------------

// LoRa transceiver
RH_RF95 LoRa(LoRa_CS, LoRa_INT);
// Array of servo objects
Servo servos[NUM_SERVO];
// // Load cell object
HX711 loadCell;
// Struct to send telemetry to ground station
TelemetryPacket telemetry = { 0 };
// Struct to send commands to the rocket
CommandPacket command = { 0 };

// -------------------------------
// Initializing Key Variables
// -------------------------------

// Used for calibrating sensors
float PT_MinV[NUM_PT] = { 0.0, 0.0, 0.0 };
// // Used for taring load cell
long loadCellOffset = 0;  // Number of bins offset
// // Used for attaching and detaching servo motors to arduino pins (reduces twitchiness)
bool servoAttachState[NUM_SERVO] = { false, false, false, false };
uint16_t servoActuationStart[NUM_SERVO] = { 0, 0, 0, 0 };
// // Initializing previous command packet to eliminate duplicate sent commands
CommandPacket lastCommand = { 0 };
// // Initializing failsafe to close servos if no commands are sent
uint16_t lastCommandTime = 0;
uint16_t lastTelemetrySent = 0;


// -------------------------------

// Main Setup

// -------------------------------

void setup() {



  // Opening Serial Monitor (For Debugging)
  Serial.begin(115200);  // Baud Rate bits/s
  delay(1000);
  // Initializing the LoRa Transceiver
  pinMode(LoRa_RST, OUTPUT);
  digitalWrite(LoRa_RST, HIGH);


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


  Serial.println("Configuring LoRa Parameters...");
  LoRa.setFrequency(915.0);  // Match ground station!
  LoRa.setTxPower(19, false);
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setModeRx();  // Explicitly start listening

  // Initializing Sensors
  initSensors(loadCell, loadCellOffset, PT_MinV);

  // Initializing Actuators
  initActuators(servos);

  // OpenLog
  initLogger();


  // Printing to Console State
  Serial.println("Rocket-Side Arduino Ready");
}


// -------------------------------
// Main Loop
// -------------------------------
void loop() {

  // Reading Sensors
  readSensors(loadCell, loadCellOffset, PT_MinV, telemetry);

  // Reading Actuator States
  readActuatorStates(servos, telemetry);

  // Reading Current Time
  telemetry.timestamp = millis();

  // ---------------- PACED LOGGING ----------------
  // Only write to the SD card every 50ms (20 Hz) to prevent 9600 baud buffer overflow
  static unsigned long lastLogTime = 0;
  if (millis() - lastLogTime >= 50) {
    logTelemetry(telemetry);
    lastLogTime = millis();
  }
  // -----------------------------------------------

  bool avail_flag = false;  // Default state is false
  if (LoRa.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (LoRa.recv(buf, &len)) {
      // 1. Check if the received length matches our struct size
      if (len != sizeof(CommandPacket)) {
        Serial.println("Error: Packet size mismatch");
        avail_flag = false;
      } else {
        // 2. Temporarily hold data to verify CRC (ONLY runs if size is correct)
        CommandPacket temp;
        memcpy(&temp, buf, sizeof(CommandPacket));

        // 3. Extract and verify CRC
        uint16_t sentCRC = temp.crc;
        temp.crc = 0;  // Clear it for calculation
        uint16_t computedCRC = computeCRC16((uint8_t*)&temp, sizeof(CommandPacket) - sizeof(temp.crc));

        if (sentCRC == computedCRC) {
          lastCommand = command;  // Save previous state
          command = temp;         // Update current state
          command.crc = sentCRC;  // Put the CRC back for debugging

          debugReceive(command);
          avail_flag = true;  // Packet is verified and ready!
        } else {
          Serial.println("Error: CRC Failure");
          avail_flag = false;
        }
      }
    }
  }

  // Acting on Commands
  // If statement is not entered if no commands were received or packets did not pass crc check
  if (avail_flag) {
    applyActuateCommands(servos, servoAttachState, servoActuationStart, lastCommand, command);

    // Taring load cell if commanded
    if (command.tareLoadCellState != lastCommand.tareLoadCellState && command.tareLoadCellState == true) {
      loadCellOffset = tareLoadCell(loadCell);
    }
    sendTelemetry(LoRa, telemetry);
  }

  updateServos(servos, servoAttachState, servoActuationStart);  // always detach servos 250 ms after actuating
}