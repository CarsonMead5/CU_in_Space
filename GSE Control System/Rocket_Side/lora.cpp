#include "lora.h"

// -------------------------------
// initLoRa() Function
// -------------------------------
void initLoRa(RH_RF95 &LoRa)
{
  // 1. Explicitly hold the reset pin HIGH to keep it awake
  pinMode(LoRa_RST, OUTPUT);
  digitalWrite(LoRa_RST, HIGH);
  // 3. The 10ms hardware reset pulse
  digitalWrite(LoRa_RST, LOW);
  delay(10);
  digitalWrite(LoRa_RST, HIGH);
  delay(10);

  // 4. Start communication
  SPI.begin();
  delay(1000);

  if (!LoRa.init())
  {
    Serial.println("LoRa Initialization Failed");
    while (1);
  }

  // Setting radio parameters to match Ground Station
  LoRa.setFrequency(LoRa_Freq);
  LoRa.setTxPower(19, false);
  LoRa.setSpreadingFactor(7);     
  LoRa.setSignalBandwidth(125E3); 
  LoRa.setCodingRate4(5);         

  // Ensure we start in RX mode
  LoRa.setModeRx();
}

// -------------------------------
// receiveCommands() Function
// -------------------------------
// Logic: Listen for ground, verify size and CRC, then copy to main struct
bool receiveCommands(RH_RF95 &LoRa, CommandPacket &cmd, CommandPacket &lastCmd)
{
  if (LoRa.available())
  {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if(LoRa.recv(buf, &len))
    {
      // 1. Check if the received length matches our struct size
      if (len != sizeof(CommandPacket)) {
        Serial.println("Error: Packet size mismatch");
        return false;
      }

      // 2. Temporarily hold data to verify CRC
      CommandPacket temp;
      memcpy(&temp, buf, sizeof(CommandPacket));

      // 3. Extract and verify CRC
      uint16_t sentCRC = temp.crc;
      temp.crc = 0; // Clear it for calculation
      uint16_t computedCRC = computeCRC16((uint8_t*)&temp, sizeof(CommandPacket) - sizeof(temp.crc));

      if (sentCRC == computedCRC)
      {
        lastCmd = cmd; // Save previous state
        cmd = temp;    // Update current state
        cmd.crc = sentCRC; // Put the CRC back for debugging

        debugReceive(cmd);
        return true;
      }
      else
      {
        Serial.println("Error: CRC Failure");
      }
    }
  }
  return false;
}

// -------------------------------
// sendTelemetry() Function
// -------------------------------
void sendTelemetry(RH_RF95 &LoRa, const TelemetryPacket &t)
{
  // Using the logic from the first file: Send -> Wait -> Set Mode RX
  Serial.println("Sending Telemetry Packet...");
  
  LoRa.send((uint8_t*)&t, sizeof(t));
  LoRa.waitPacketSent();

  // CRITICAL: Return to RX mode so we can hear the NEXT command
  LoRa.setModeRx();

  debugSend(t);
}

// -------------------------------
// debugReceive() & debugSend()
// -------------------------------
// (Kept your original logic but ensured it uses the correct struct members)

void debugReceive(const CommandPacket &cmd)
{
  Serial.println("Commands Received:");
  Serial.print("packetID: ");
  Serial.print(cmd.packetID);
  Serial.print(" | Servo States: ");
  for (uint8_t i=0; i<NUM_SERVO; i++)
  {
    Serial.print(cmd.servoState[i]);
    Serial.print(",");
  }
  Serial.print(" | Solenoid State: ");
  Serial.print(cmd.solenoidState);
  Serial.print(" | Armed State: ");
  Serial.print(cmd.armedState);
  Serial.print(" | Ematch States: ");
  for (uint8_t i=0; i<NUM_EMATCH; i++)
  {
    Serial.print(cmd.ematchState[i]);
  }
  Serial.print(" | Tare Load Cell State: ");
  Serial.print(cmd.tareLoadCellState);
  Serial.print(" | CRC: ");
  Serial.println(cmd.crc);
}

void debugSend(const TelemetryPacket &t)
{
  Serial.println("Telemetry Sent:");
  Serial.print("Pressures: ");
  for (uint8_t i=0; i<NUM_PT; i++)
  {
    Serial.print(t.pressure[i]);
    Serial.print(",");
  }
  Serial.print(" | Load Cell: ");
  Serial.print(t.loadCell);
  Serial.print(" | Servo Positions: ");
  for (uint8_t i=0; i<NUM_SERVO; i++)
  {
    Serial.print(t.servoPos[i]);
    Serial.print(",");
  }
  Serial.print(" | Solenoid State: ");
  Serial.print(t.solenoidState);
  Serial.print(" | Timestamp: ");
  Serial.println(t.timestamp);
}







