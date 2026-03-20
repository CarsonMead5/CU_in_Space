#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define RF95_FREQ 915.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

// --- STRUCT DEFINITIONS ---
struct __attribute__((packed)) TelemetryPacket 
{
  uint32_t pressure[3]; // Pressure measurements (psi) *100
  uint32_t loadCell;    // Load cell measurement (lb) *100
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

TelemetryPacket outgoingTelem = { {0, 0, 0}, 0, {0, 0, 0, 0}, false, 0 };

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

    // 1. RECEIVE: Look for CommandPacket size
    if (rf95.recv(buf, &len) && len == sizeof(CommandPacket)) {
      
      memcpy(&incomingCommand, buf, sizeof(CommandPacket));

      // --- PRINT ALL VALUES ---
      Serial.println("--- NEW COMMAND RECEIVED ---");
      Serial.print("Packet ID:    "); Serial.println(incomingCommand.packetID);
      
      // Print Servo States
      Serial.print("Servos:       ");
      for(int i = 0; i < 4; i++) {
        Serial.print(incomingCommand.servoState[i]); 
        if(i < 3) Serial.print(", ");
      }
      Serial.println();

      // Print Single States
      Serial.print("Solenoid:     "); Serial.println(incomingCommand.solenoidState);
      Serial.print("Armed:        "); Serial.println(incomingCommand.armedState);

      // Print E-Match States
      Serial.print("E-Matches:    ");
      for(int i = 0; i < 2; i++) {
        Serial.print(incomingCommand.ematchState[i]);
        if(i < 1) Serial.print(", ");
      }
      Serial.println();

      Serial.print("Tare Load:    "); Serial.println(incomingCommand.tareLoadCellState);
      Serial.print("CRC:          "); Serial.println(incomingCommand.crc);
      Serial.println("---------------------------");

      for(int i = 0; i < 3; i++) outgoingTelem.pressure[i] += 10;
      outgoingTelem.loadCell += 5;
      for(int i = 0; i < 4; i++) outgoingTelem.servoPos[i] += 1;
      outgoingTelem.solenoidState = !outgoingTelem.solenoidState; // Toggle for test
      outgoingTelem.timestamp = millis();

      // --- 2. RESPOND WITH ROCKET STRUCT ---
      Serial.println("Sending Telemetry Packet...");
      
      rf95.send((uint8_t*)&outgoingTelem, sizeof(outgoingTelem));
      rf95.waitPacketSent();

      rf95.setModeRx(); // Back to listening
      
      Serial.println("Response Sent.");
    }
  }
}