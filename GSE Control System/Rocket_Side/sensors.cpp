#include "sensors.h"

// Variable to track the latest raw value in the background
long lastRawLoadCell = 0;

// -------------------------------
// initSensors() Function
// -------------------------------
// Initializing all sensors (pressure transducers, load cell)
void initSensors(HX711 &loadCell, long &loadCellOffset, float PT_MinV[]) {
  // Serial.println("sensors 9");
  // Initializing load cell communication through HX711
  Serial.println("    Beginning Load Cell...");
  loadCell.begin(loadCell_DT, loadCell_CLK);
  // Serial.println("sensors 12");

  // Taring load cell
  loadCellOffset = tareLoadCell(loadCell);
  // Serial.println("sensors 16");

  // Calibrating pressure transducers
  Serial.println("    Calibrating PTs... (this does nothing rn)");
  for (uint8_t i = 0; i < NUM_PT; i++) {
    PT_MinV[i] = PT_HardVMin[i];
    // calibratePressure(pressurePins[i]);
  }
  Serial.println("sensors 24");
}

// -------------------------------
// readSensors() Function
// -------------------------------
void readSensors(HX711 &loadCell, long &loadCellOffset, float PT_MinV[], TelemetryPacket &t) {
  // --- NON-BLOCKING LOAD CELL READ ---
  if (loadCell.is_ready()) {
    // 1. Instantly grab the latest single value and save it to our global tracker
    lastRawLoadCell = loadCell.read(); // <--- UPDATED
    long corrected = lastRawLoadCell - loadCellOffset;
    float current_force = corrected / loadCellCalibrationFactor;

    // 2. Apply a software smoothing filter (Exponential Moving Average)
    static float filtered_force = 0.0;
    filtered_force = (0.3 * current_force) + (0.7 * filtered_force);

    // 3. Store in telemetry packet
    t.loadCell = (int32_t)(filtered_force * 100);
  }

  // If the HX711 isn't ready yet, t.loadCell simply keeps its previous value and the loop keeps going!

  // --- PRESSURE TRANSDUCERS ---
  // analogRead takes ~0.1 milliseconds, so doing 3 of them is totally fine
  for (uint8_t i = 0; i < NUM_PT; i++) {
    // 1. Read the raw ADC value and convert to Volts using config definitions
    int rawADC = analogRead(pressurePins[i]);
    float voltage = (rawADC / ADC_Resolution) * PT_RefV;

    // 2. Calculate the specific slope (psi/V) for this transducer
    // Slope = Max Pressure / Voltage Range
    float voltageRange = PT_MaxV - PT_MinV[i];
    float slope = PT_MaxP[i] / voltageRange;

    // 3. Subtract the baseline, then multiply by the calculated slope
    float pressure = (voltage - PT_MinV[i]) * slope; 

    if (pressure < 0) {
      pressure = 0;
    }

    // Store in telemetry packet
    t.pressure[i] = pressure * 100;

    // // Printing out for debugging
    // Serial.print("Pressure Reading ");
    // Serial.print(i);
    // Serial.print(": ");
    // Serial.print((float)t.pressure[i] / 100.0);
    // Serial.print("   ");
    // if (i == 2) {
    //   Serial.println();
    // }
  }
}
// -------------------------------
// calibratePressure() Function // ignore this whole john it does nothing
// -------------------------------
// Calibrating pressure sensors (returning voltage where psi = 0);
float calibratePressure(uint8_t Pin_Num) {
  // Averaging out many pressure readings (bin)
  long sum = 0;
  for (uint8_t i = 0; i < 50; i++) {
    sum += analogRead(Pin_Num);
  }
  float avgADC = sum / 50.0;

  // Printing out zeroed bin number
  Serial.print("Average PT ADC Calculated: ");
  Serial.println(avgADC);

  // Calculating voltage where pressure is 0 psi (gauge)
  // Updated to use the 5.31V reference and 1024.0 divisor
  float zeroV = (avgADC / 1024.0) * 5.31;

  // Returning 0 psi voltage measurement
  return zeroV;
}

// -------------------------------
// tareLoadCell() Function
// -------------------------------
// Taring the load cell instantly without blocking the main loop
long tareLoadCell(HX711 &loadCell) {
  // Instead of read_average(10) which freezes the Arduino, 
  // we just instantly return the most recent raw reading!
  return lastRawLoadCell;
}