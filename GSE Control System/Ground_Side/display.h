#ifndef DISPLAY_H
#define DISPLAY_H

#include "packets.h"
#include "config.h"
// #include <Wire.h> 
// #include <LiquidCrystal_I2C.h> 
#include <LedControl.h> // Controls the 7 Segment Displays

// Key LCD Variables (Commented out for now)
// constexpr uint16_t LCD_Address[NUM_LCD] = {0x20,0x21};

void initDisplays(LedControl &SevSD);

void updateDisplays(const TelemetryPacket &t, LedControl &SevSD);

// Added an optional decimal position parameter (defaults to -1, which means no decimal)
void displayNumber7SD(LedControl &SevSD, uint8_t device, long value, int dpPosition = -1);

#endif