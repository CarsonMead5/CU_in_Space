#ifndef LOG_H
#define LOG_H

#include "packets.h"
#include "config.h"

// Arduino Serial 1 goes to Pin 18 and Pin 19
// Open Log PINOUT: TX -> Pin 19, RX -> Pin 18, 5V Power, GND

// Initializing data logger
void initLogger();

// new file auto starts itself

// Logs the telemetry received to the microSD card
void logTelemetry(const TelemetryPacket &t);


#endif