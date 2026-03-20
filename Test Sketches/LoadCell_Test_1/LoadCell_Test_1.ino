#include "HX711.h"

// Defining the ports that
#define DAT 3 // DT
#define CLK 2 // SCK

// Creating load cell object
HX711 load_cell;

// Initializing Load Cell Constants
long offset = 0;
float calibration_factor = 4299.18; // BinNumber/kg (EXPERIMENTALLY FOUND)

void setup() {
  // Starting serial port
  Serial.begin(9600);

  // Initializing the connection with the HX711
  load_cell.begin(DAT, CLK);

  // Waiting for the HX711 to pair correctly
  while (!load_cell.is_ready()) {
    Serial.println("Waiting for HX711");
    delay(100);
  }

  delay(1000); // Letting amplifier settle

  // Taring the load_cell
  Serial.println("Remove all load...");
  delay(3000);

  offset = load_cell.read_average(20);
  Serial.print("Offset value: ");
  Serial.println(offset);
}

void loop() {

  long raw = load_cell.read_average(10);
  long corrected = raw-offset;

  // Converting bin number into usable values
  float force_kg = corrected / calibration_factor;
  float force_lb = force_kg * 2.20462;

  Serial.print("Force: ");
  Serial.print(force_lb,3);
  Serial.println(" lb");
}
