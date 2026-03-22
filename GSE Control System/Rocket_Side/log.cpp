#include "log.h"

// -------------------------------
// initLogger() Function
// -------------------------------
void initLogger()
{
  // Opening up serial channel 1 (9600 baud)
  Serial1.begin(115200);
  
  // Give the OpenLog 1 second to wake up and generate its automatic LOGxxxxx.TXT file
  delay(1000);

  // Entering csv header
  Serial1.println("Time(ms),PT0(psi),PT1(psi),PT2(psi),LoadCell(lb)");
}

// -------------------------------
// logTelemetry() Function
// -------------------------------
void logTelemetry(const TelemetryPacket &t)
{
  // Timestamp
  Serial1.print(t.timestamp);
  Serial1.print(",");

  // Pressures (Divide by 100.0 to keep decimals)
  for (uint8_t i=0; i<NUM_PT; i++)
  {
    Serial1.print(t.pressure[i] / 100.0, 2); 
    Serial1.print(",");
  }

  // Load Cell (Cast to int32_t to prevent 4-billion underflow error, keep decimals)
  float thrust = (int32_t)t.loadCell / 100.0;
  Serial1.print(thrust, 2);
  
  // End the row
  Serial1.println();
}