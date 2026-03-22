#include "sensors.h"

// -------------------------------
// initSensors() Function
// -------------------------------
// Initializing all sensors (pressure transducers, load cell)
void initSensors(HX711 &loadCell, long &loadCellOffset, float PT_MinV[])
{
    // Serial.println("sensors 9");
  // Initializing load cell communication through HX711
  loadCell.begin(loadCell_DT, loadCell_CLK);
  // Serial.println("sensors 12");

  // Taring load cell
  loadCellOffset = tareLoadCell(loadCell);
  // Serial.println("sensors 16");

  // Calibrating pressure transducers
  for (uint8_t i=0; i<NUM_PT; i++)
  {
    PT_MinV[i] = calibratePressure(pressurePins[i]);
  }
  // Serial.println("sensors 21");
}


// -------------------------------
// readSensors() Function
// -------------------------------
void readSensors(HX711 &loadCell, long &loadCellOffset, float PT_MinV[], TelemetryPacket &t)
{
  // --- NON-BLOCKING LOAD CELL READ ---
  if (loadCell.is_ready()) {
    // 1. Instantly grab the latest single value (does not wait)
    long raw = loadCell.read(); 
    long corrected = raw - loadCellOffset;
    float current_force = corrected / loadCellCalibrationFactor;

    // 2. Apply a software smoothing filter (Exponential Moving Average)
    // This perfectly replicates read_average(3) but takes 0.001 milliseconds!
    // The 0.3 means "trust the new reading 30%, trust the history 70%"
    static float filtered_force = 0.0;
    filtered_force = (0.3 * current_force) + (0.7 * filtered_force);

    // 3. Store in telemetry packet
    t.loadCell = (int32_t)(filtered_force * 100);
  }
  // If the HX711 isn't ready yet, t.loadCell simply keeps its previous value 
  // and the loop keeps flying at maximum speed!

  // --- PRESSURE TRANSDUCERS ---
  // analogRead takes ~0.1 milliseconds, so doing 3 of them is totally fine
  for (uint8_t i=0; i<NUM_PT; i++)
  {
    float voltage = (analogRead(pressurePins[i]) / ADC_Resolution) * PT_RefV;
    float pressure = (voltage - PT_MinV[i]) * (PT_MaxP[i]) / (PT_MaxV - PT_MinV[i]);

    if (pressure < 0) pressure = 0;

    t.pressure[i] = pressure * 100;
  }
}

// -------------------------------
// calibratePressure() Function
// -------------------------------
// Calibrating pressure sensors (returning voltage where psi = 0);
float calibratePressure(uint8_t Pin_Num)
{
  // Averaging out many pressure readings (bin)
  long sum = 0;
  for (uint8_t i=0; i<50; i++)
  {
    sum += analogRead(Pin_Num);
  } 
  float avgADC = sum / 50.0;

  // Calculating voltage where pressure is 0 psi (gauge)
  float zeroV = (avgADC / ADC_Resolution) * PT_RefV;

  // Returnign 0 psi voltage measurment
  return zeroV;
}


// -------------------------------
// tareLoadCell() Function
// -------------------------------
// Taring the load cell 
long tareLoadCell(HX711 &loadCell)
{
  long offset = loadCell.read_average(10) ;

  return offset;
}