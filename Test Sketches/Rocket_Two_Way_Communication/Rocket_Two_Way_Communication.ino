#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define RF95_FREQ 915.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

float responseValue = 100.0;

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

  // Match settings exactly
  rf95.setSpreadingFactor(7);
  rf95.setSignalBandwidth(125E3);
  rf95.setCodingRate4(5);

  rf95.setModeRx();

  Serial.println("Rocket Ready");
}

void loop() {
  if (rf95.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len) && len == sizeof(float)) {
      float receivedValue;
      memcpy(&receivedValue, buf, sizeof(receivedValue));

      Serial.print("Received: ");
      Serial.println(receivedValue);

      // ---------------- RESPOND ----------------
      Serial.print("Sending back: ");
      Serial.println(responseValue);

      rf95.send((uint8_t*)&responseValue, sizeof(responseValue));
      rf95.waitPacketSent();

      // CRITICAL
      rf95.setModeRx();

      responseValue += 1.0f;
    }
  }
}