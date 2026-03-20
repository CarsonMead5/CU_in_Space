#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define RF95_FREQ 915.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

// --- STRUCT DEFINITIONS ---
struct __attribute__((packed)) GroundTXPacket {
  bool bool1; 1 to 5, 7 8
  bool bool2;
};

struct __attribute__((packed)) RocketTXPacket {
  float float1; 
  float float2;    
};

// Instances for handling data
GroundTXPacket incomingGroundData;
RocketTXPacket rocketData = {0.0, 0.0};

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

    // 1. RECEIVE: Look for GroundTXPacket size
    if (rf95.recv(buf, &len) && len == sizeof(GroundTXPacket)) {
      memcpy(&incomingGroundData, buf, sizeof(GroundTXPacket));

      Serial.print("Received from Ground! B1: ");
      Serial.print(incomingGroundData.bool1);
      Serial.print(" B2: ");
      Serial.println(incomingGroundData.bool2);

      // --- UPDATE ROCKET DATA ---
      // Update your floats based on sensors or logic
      rocketData.float1 += 0.1; 
      rocketData.float2 = 12.4; // Example constant

      // --- 2. RESPOND WITH ROCKET STRUCT ---
      Serial.println("Sending Rocket Packet...");
      
      rf95.send((uint8_t*)&rocketData, sizeof(rocketData));
      rf95.waitPacketSent();

      // RITICAL: Switch back to RX mode immediately
      rf95.setModeRx();
      
      Serial.println("Response Sent.");
    }
  }
}