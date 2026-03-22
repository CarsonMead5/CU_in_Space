#include "display.h"

// Define the LCD objects using addresses from config.h
LiquidCrystal_I2C lcd1(LCD_Address[0], 16, 2);
LiquidCrystal_I2C lcd2(LCD_Address[1], 16, 2);

// -------------------------------
// initDisplays()
// -------------------------------
void initDisplays(LedControl &SevSD) {
  // 1. 7-Segment Setup
  for (int i = 0; i < NUM_SevSD; i++) {
    SevSD.shutdown(i, false);
    SevSD.setIntensity(i, 8);  // Slightly brighter for outdoor use
    SevSD.clearDisplay(i);
  }

  // 2. LCD Setup
  lcd1.init();
  lcd1.backlight();
  lcd2.init();
  lcd2.backlight();

  // Print Static Labels once to prevent flickering
  lcd1.setCursor(0, 0);
  lcd1.print("LC:");
  lcd1.setCursor(0, 1);
  lcd1.print("PT0:   ");

  lcd2.setCursor(0, 0);
  lcd2.print("PT1:   ");
  lcd2.setCursor(0, 1);
  lcd2.print("PT2:   ");
}

// -------------------------------
// updateDisplays()
// -------------------------------
void updateDisplays(const TelemetryPacket &t, LedControl &SevSD) {
  static unsigned long lastDisplayUpdate = 0;
  if (millis() - lastDisplayUpdate < 150) return;
  lastDisplayUpdate = millis();

  // Update 7-Segment Displays
  displayNumber7SD(SevSD, 0, (long)(t.loadCell), 2);
  displayNumber7SD(SevSD, 1, (long)(t.pressure[0]));
  displayNumber7SD(SevSD, 2, (long)(t.pressure[1]));
  displayNumber7SD(SevSD, 3, (long)(t.pressure[2]));

  // Update LCD 1 (0x3E)
  lcd1.setCursor(8, 0);

  // 1. Cast the unsigned telemetry value to a SIGNED 32-bit integer
  // 2. Divide by 100.0 to get the decimal
  float thrustVal = (int32_t)t.loadCell / 100.0;
  // 3. Print the result
  lcd1.print(thrustVal, 2);
  lcd1.print(" lb  ");  // The extra spaces "wipe" old digits

  lcd1.setCursor(8, 1);
  lcd1.print(t.pressure[0] / 100.0, 1);
  lcd1.print(" psi ");

  // Update LCD 2 (0x27)
  lcd2.setCursor(8, 0);
  lcd2.print(t.pressure[1] / 100.0, 1);
  lcd2.print(" psi ");

  lcd2.setCursor(8, 1);
  lcd2.print(t.pressure[2] / 100.0, 1);
  lcd2.print(" psi ");
}

// -------------------------------
// displayNumber7SD() - The missing piece!
// -------------------------------
void displayNumber7SD(LedControl &SevSD, uint8_t device, long value, int dpPosition) {
  SevSD.clearDisplay(device);

  bool isNegative = false;
  if (value < 0) {
    isNegative = true;
    value = -value;
  }

  // Handle pure zero
  if (value == 0 && !isNegative && dpPosition < 0) {
    SevSD.setDigit(device, 0, 0, false);
    return;
  }

  int digitPos = 0;
  int minDigits = (dpPosition >= 0) ? (dpPosition + 1) : 1;

  while ((value > 0 || digitPos < minDigits) && digitPos < 8) {
    bool showDP = (digitPos == dpPosition);
    SevSD.setDigit(device, digitPos, value % 10, showDP);
    value /= 10;
    digitPos++;
  }

  if (isNegative && digitPos < 8) {
    SevSD.setChar(device, digitPos, '-', false);
  }
}