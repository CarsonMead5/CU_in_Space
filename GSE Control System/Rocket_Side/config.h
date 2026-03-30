#ifndef CONFIG_H
#define CONFIG_H

#include "packets.h"

// -------------------------------
// LoRa Configuration
// -------------------------------

// VIN->5V, GND->GND, EN hangs, G0(INT)->2, SCK->52, MISO->50, MOSI->51, CS->10, RST->9
constexpr uint8_t LoRa_INT = 2;
constexpr uint8_t LoRa_RST = 9;
constexpr uint8_t LoRa_CS = 10;

// -------------------------------
// Actuator Configurations
// -------------------------------
// Servo1 = Fill -> 3
// Servo2 = Tank -> 4
// Servo3 = Vent -> 5
// Servo4 = Dump -> 6
// Solenoid -> 8
// Ematch1 -> 26
// Ematch2 -> 28
const uint8_t servoPins[NUM_SERVO] = {3,4,5,6};
const uint32_t servoLOW[NUM_SERVO] = {500, 500, 500, 500 }; 
const uint32_t servoHIGH[NUM_SERVO] = {1000, 1000, 1000, 1000}; 
constexpr uint8_t solenoidPin = 35;
constexpr uint8_t ematchPins[NUM_EMATCH] = {34,28};

// -------------------------------
// Sensor Configurations
// -------------------------------

// Pressure Transducer Signal Pins:
// PT1 -> A0
// PT2 -> A1
// PT3 -> A2

// Load Cell: DAT -> 11, CLK -> 7
const uint8_t pressurePins[NUM_PT] = {A0,A1,A2};
constexpr uint8_t loadCell_DT = 11;
constexpr uint8_t loadCell_CLK = 7;

#endif