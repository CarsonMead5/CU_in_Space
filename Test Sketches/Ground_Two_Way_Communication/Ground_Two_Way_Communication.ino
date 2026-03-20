#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define RF95_FREQ 915.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

struct __attribute__((packed)) TelemetryPacket 
{
  uint32_t pressure[3]; // Pressure measurements (psi) *100 (lessens storage space and keeps 2 decimal places)
  uint32_t loadCell; // Load cell measurement (lb) *100 (lessens storage space and keeps 2 decimal places)
  uint8_t servoPos[4];
  bool solenoidState;
  uint32_t timestamp;
};

struct __attribute__((packed)) CommandPacket
{
  uint16_t packetID;
  bool servoState[4];
  bool solenoidState;
  bool armedState;
  bool ematchState[2];
  bool tareLoadCellState;
  uint16_t crc;
};


CommandPacket command = {0};
TelemetryPacket telemetry;


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

  rf95.send((uint8_t*)&command, sizeof(command));
  rf95.waitPacketSent();

  // CRITICAL
  rf95.setModeRx();

  // ---------------- RECEIVE ----------------
  unsigned long start = millis();
  while (millis() - start < 200) { // wait up to 200 ms
    if (rf95.available()) {
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);

      if (rf95.recv(buf, &len) && len == sizeof(TelemetryPacket)) {

        memcpy(&telemetry, buf, sizeof(TelemetryPacket));

        Serial.print("Received: ");
        for (uint8_t i=0; i<3; i++)
        {
        Serial.print(telemetry.pressure[i]);
        Serial.print(",");
        }
        Serial.print(telemetry.loadCell);
        Serial.print(",");
        for (uint8_t i=0; i<4; i++)
        {
        Serial.print(telemetry.servoPos[i]);
        Serial.print(",");
        }
        Serial.print(telemetry.solenoidState);
        Serial.print(",");
        Serial.println(telemetry.timestamp);
        break;
      }
    }
  }

  // Incrementing command packet
  command.packetID++; 
  command.solenoidState = !command.solenoidState; // Toggle state
  command.armedState = (command.packetID % 3 == 0); // Arm every 3rd packet
  
  delay(100);
}