#include "HX711.h"

#define DAT 3
#define CLK 2

HX711 scale;

long offset = 0;

void setup() {
  Serial.begin(9600);
  scale.begin(DAT, CLK);

  Serial.println("Waiting for HX711");

  while(!scale.is_ready()) {
    delay(10);
  }

  delay(1000);
  Serial.println("Remove all load...");
  delay(3000);

  offset = scale.read_average(20); // manual tare
  Serial.print("Offset Value: ");
  Serial.println(offset);

  Serial.println("Place known weight on load cell...");
}

// Loop is to determine the calibration factor of the load cell
// THIS IS EXPERIMENTALLY DRIVEN
void loop() {
  long raw = scale.read_average(10);
  long corrected = raw-offset;

  Serial.print("Zeroed Reading: ");
  Serial.println(corrected);

  delay(100);
}
