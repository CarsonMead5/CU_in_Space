#include "display.h"

// -------------------------------
// initDisplays()
// -------------------------------
void initDisplays(LedControl &SevSD) {
  // 7SD Setup (All 4 Modules in Series)
  for (int i = 0; i < NUM_SevSD; i++) {
    SevSD.shutdown(i, false);
    SevSD.setIntensity(i, 5); // Mid-level brightness
    SevSD.clearDisplay(i);
  }
}

// -------------------------------
// updateDisplays()
// -------------------------------
void updateDisplays(const TelemetryPacket &t, LedControl &SevSD) {
  static unsigned long lastDisplayUpdate = 0;

  // Only updating every 1/10th of a second
  if(millis() - lastDisplayUpdate < 100) return;
  lastDisplayUpdate = millis();

  // Load Cell: Pass '2' at the end to turn on the decimal point 2 digits in (0.00 accuracy)
  displayNumber7SD(SevSD, 0, (long)(t.loadCell), 2);
  
  // Pressures: No decimal parameter passed, so they default to whole numbers
  displayNumber7SD(SevSD, 1, (long)(t.pressure[0]));
  displayNumber7SD(SevSD, 2, (long)(t.pressure[1]));
  displayNumber7SD(SevSD, 3, (long)(t.pressure[2]));
}

// -------------------------------
// displayNumber7SD()
// -------------------------------
void displayNumber7SD(LedControl &SevSD, uint8_t device, long value, int dpPosition) {
  SevSD.clearDisplay(device);

  bool isNegative = false;
  if (value < 0) {
    isNegative = true;
    value = -value;
  }

  // Handle pure zero when NO decimal is needed
  if (value == 0 && !isNegative && dpPosition < 0) {
    SevSD.setDigit(device, 0, 0, false);
    return;
  }

  int digitPos = 0;
  
  // Calculate the minimum digits we MUST print to reach the decimal point
  // (e.g., if dpPosition is 2, we must print at least 3 digits to get "0.00")
  int minDigits = (dpPosition >= 0) ? (dpPosition + 1) : 1;

  // Keep printing if we have numbers left, OR if we haven't reached the decimal padding
  while ((value > 0 || digitPos < minDigits) && digitPos < 8) {
    
    // Check if the current digit index matches our requested decimal point location
    bool showDP = (digitPos == dpPosition);
    
    SevSD.setDigit(device, digitPos, value % 10, showDP); 
    value /= 10;                                         
    digitPos++;
  }

  // Slap the negative sign at the very end (left side)
  if (isNegative && digitPos < 8) {
    SevSD.setChar(device, digitPos, '-', false);
  }
}