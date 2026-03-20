// Initializing pressure transducer pin
const uint8_t PRESSURE_PIN = A0;

// Arduio Board Specifications
const float ADC_RESOLUTION = 1023.0; // 10 bit processor (0-1023)
const float REFERENCE_VOLTAGE = 5.0;

// Sensor Specs
float zeroVoltage; // V
const float V_MAX = 4.5; // V
const float P_MIN = 0.0; // psi
const float P_MAX = 1600.0; // psi

// Function to read the voltage output by the sensor
float readVoltage() {
  long sum = 0;

  // Summing 20 different readings (bin number)
  for (int i = 0; i < 20; i++){
    sum += analogRead(PRESSURE_PIN);
  }

  // Averaging the 20 readings (bin number)
  float avgADC = sum / 20.0;

  // Converting the bin number to a voltage
  return (avgADC / ADC_RESOLUTION) * REFERENCE_VOLTAGE;
}


// Function to calibrate the zero value of the pressure transducer
void calibrateZero() {
  long sum = 0;
  for (int i = 0; i < 100; i++) {
    sum += analogRead(PRESSURE_PIN);
    delay(5);
  }
  // Averaging the 100 readings
  float avgADC = sum / 100.0;

  // Changing the zeroed voltage to be the measured zero
  zeroVoltage = (avgADC / ADC_RESOLUTION) * REFERENCE_VOLTAGE;

  Serial.print("Zeroed Voltage: ");
  Serial.print(zeroVoltage, 3);
  Serial.println(" V");
}

void setup() {
  Serial.begin(9600);
  Serial.println("1600 PSI Pressure Sensor Test");

  // Calibrating the sensor
  calibrateZero();
}

void loop() {

  // Calling readVoltage() function to read an average of 20 measurement readings
  float voltage = readVoltage();

  // Calculating the current pressure reading
  float pressure = (voltage - zeroVoltage) * (P_MAX-P_MIN) / (V_MAX - zeroVoltage);

  // Making bounds for the pressure data (odd voltage readings)
  if (pressure < 0) pressure = 0;
  if (pressure > P_MAX) pressure = P_MAX;

  // Printing read pressure to serial monitor
  Serial.print("Voltage: ");
  Serial.print(voltage, 3); // 3 Decimal Places
  Serial.print(" V   Pressure: ");
  Serial.print(pressure, 1);
  Serial.println(" PSI");

  // Delaying 500 ms
  delay(500);
}
