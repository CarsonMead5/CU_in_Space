#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9,10);

void setup() {

  Serial.begin(115200);
  pinMode(53, OUTPUT);   // required for Mega

  Serial.println("NRF24 Diagnostic Test");

  if (!radio.begin()) {
    Serial.println("ERROR: Radio hardware not responding!");
    while(1);
  }

  Serial.println("Radio detected.");

  if (radio.isChipConnected()) {
    Serial.println("SPI communication with radio is GOOD.");
  } else {
    Serial.println("SPI communication FAILED.");
  }

  Serial.println("Register test:");

  for (int i = 0; i < 10; i++) {
    byte reg = radio.printStatus();
    Serial.print("STATUS register = 0x");
    Serial.println(reg, HEX);
    delay(1000);
  }
}

void loop() {}
