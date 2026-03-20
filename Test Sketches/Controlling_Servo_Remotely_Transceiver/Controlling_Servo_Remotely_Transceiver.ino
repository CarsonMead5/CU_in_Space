// Code for Controlling a Servo Motor Remotely Transmitter Side
// Contributors - Carson Mead

// Including Necessary Libraries
#include <RH_RF95.h>
#include <SPI.h>

// Defining LoRa and Switch Pins
#define LoRa_CS 10
#define LoRa_RST 9
#define LoRa_INT 2
#define LoRa_Freq 915.0
#define Switch_Pin 35

// Initializing LoRa Object
RH_RF95 LoRa(LoRa_CS, LoRa_INT);

// Time Since Last Transmission
unsigned long LastTransmissionTime = millis();

// Main Setup
void setup() {
  
  // Initializing Switch
  pinMode(Switch_Pin, INPUT_PULLUP);

  // Initializing LoRa
  pinMode(LoRa_RST, OUTPUT);
  digitalWrite(LoRa_RST,HIGH);

  // Beginning Serial Monitor
  Serial.begin(9600);
  delay(1000);

  // Resetting LoRa
  digitalWrite(LoRa_RST, LOW);
  delay(10);
  digitalWrite(LoRa_RST, HIGH);
  delay(10);

  SPI.begin();

  // Edge Case that LoRa Initialization Failed
  if (!LoRa.init()) {
    Serial.println("LoRa Initialization Failed");
    while(1);
  }

  // Setting LoRa Parameters
  LoRa.setFrequency(LoRa_Freq);
  LoRa.setTxPower(23, false);

  // Printing that System is Ready
  Serial.println("LoRa Transmitter Ready");

}

void loop() {
  
  // Reading the State of the Switch every 200 ms
  if ((millis() - LastTransmissionTime) >= 200) {

    uint8_t command;

    // 5V if off, 0V if on (The resistor is before the pin)
    if (digitalRead(Switch_Pin) == LOW)
    {
      command = 1;
    }
    else
    {
      command = 0;
    }
    
    LoRa.send((uint8_t*)&command, 1); // boolean is 1 bit
    LoRa.waitPacketSent();

    Serial.print("Sent: ");
    Serial.println(command);

    // Resetting transmission time
    LastTransmissionTime = millis();
  }

}
