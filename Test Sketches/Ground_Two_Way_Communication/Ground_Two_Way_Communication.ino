#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define RF95_FREQ 915.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

float txValue = 0.0;

void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600);
  delay(1000);

  // Reset radio
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  SPI.begin();

  if (!rf95.init()) {
    Serial.println("LoRa init failed");
    while (1);
  }

  rf95.setFrequency(RF95_FREQ);
  rf95.setTxPower(23, false);

  // Match settings explicitly
  rf95.setSpreadingFactor(7);
  rf95.setSignalBandwidth(125E3);
  rf95.setCodingRate4(5);

  rf95.setModeRx();

  Serial.println("Ground Ready");
}

void loop() {
  // ---------------- SEND ----------------
  Serial.print("Sending: ");
  Serial.println(txValue);

  rf95.send((uint8_t*)&txValue, sizeof(txValue));
  rf95.waitPacketSent();

  // 🔴 CRITICAL
  rf95.setModeRx();

  // ---------------- RECEIVE ----------------
  unsigned long start = millis();
  while (millis() - start < 200) { // wait up to 200 ms
    if (rf95.available()) {
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);

      if (rf95.recv(buf, &len) && len == sizeof(float)) {
        float rxValue;
        memcpy(&rxValue, buf, sizeof(rxValue));

        Serial.print("Received: ");
        Serial.println(rxValue);
        break;
      }
    }
  }

  txValue += 0.5f;
  delay(500);
}