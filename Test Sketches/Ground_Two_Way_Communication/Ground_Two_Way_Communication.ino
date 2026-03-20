#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define RF95_FREQ 915.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

struct __attribute__((packed)) GroundTxPacket{
  bool bool1;
  bool bool2;
};

struct __attribute__((packed)) RocketTxPacket{
  float float1;
  float float2;
};


GroundTxPacket txPacket = {false,false};
RocketTxPacket rxPacket = {0.0,0.0};


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
  rf95.setTxPower(19, false);

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
  Serial.print(txPacket.bool1);
  Serial.print(",");
  Serial.println(txPacket.bool2);

  rf95.send((uint8_t*)&txPacket, sizeof(txPacket));
  rf95.waitPacketSent();

  // CRITICAL
  rf95.setModeRx();

  // ---------------- RECEIVE ----------------
  unsigned long start = millis();
  while (millis() - start < 200) { // wait up to 200 ms
    if (rf95.available()) {
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);

      if (rf95.recv(buf, &len) && len == sizeof(RocketTxPacket)) {

        memcpy(&rxPacket, buf, sizeof(RocketTxPacket));

        Serial.print("Received: ");
        Serial.print(rxPacket.float1);
        Serial.print(",");
        Serial.println(rxPacket.float2);
        break;
      }
    }
  }

  // Incrementing transmit packet
  if (txPacket.bool2 == false)
  {
    txPacket.bool2 = true;
  }
  else
  {
    txPacket.bool2 = false;
  }

  delay(100);
}