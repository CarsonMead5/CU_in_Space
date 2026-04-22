#include <SPI.h>
#include <RH_RF95.h>

#define LoRa_INT 2
#define LoRa_RST 9
#define LoRa_CS 10

RH_RF95 LoRa(LoRa_CS, LoRa_INT);

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for serial to connect

  Serial.println("--- LoRa Bare Minimum Test ---");

  pinMode(LoRa_RST, OUTPUT);
  digitalWrite(LoRa_RST, HIGH);
  delay(100);

  Serial.println("1. Resetting Module...");
  digitalWrite(LoRa_RST, LOW);
  delay(10);
  digitalWrite(LoRa_RST, HIGH);
  delay(50);

  Serial.println("2. Testing SPI Connection...");
  if (!LoRa.init()) {
    Serial.println("FAIL: Arduino cannot see the LoRa chip!");
    Serial.println("Check MISO/MOSI wiring, CS pin, and power.");
    while (1);
  }
  
  Serial.println("SUCCESS: LoRa Module initialized perfectly!");
}

void loop() {}