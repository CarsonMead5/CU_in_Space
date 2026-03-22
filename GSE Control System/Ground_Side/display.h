#ifndef DISPLAY_H
#define DISPLAY_H

#include "packets.h"
#include "config.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h> 
#include <LedControl.h> 

// LCD
// SDA 20
// SDL 21

// Declare LCD objects as extern so display.cpp can define them
extern LiquidCrystal_I2C lcd1;
extern LiquidCrystal_I2C lcd2;

constexpr uint16_t LCD_Address[NUM_LCD] = {0x3E, 0x27};

void initDisplays(LedControl &SevSD);
void updateDisplays(const TelemetryPacket &t, LedControl &SevSD);
void displayNumber7SD(LedControl &SevSD, uint8_t device, long value, int dpPosition = -1);

#endif