// Testing Two-Way LoRa Communication
// Contributors: Carson Mead
// Rocket Side: Load Cell Measurements and Servo Actuation

// Including Necessary Libraries
#include <RH_RF95.h>
#include <SPI.h>
#include <Servo.h>
#include <HX711.h>

// Pin Numbers
#define LoRa_CS 10
#define LoRa_RST 9
#define LoRa_INT 2
#define LoadCell_DT 4
#define LoadCell_CLK 3

// Initializing Objects
const int NumServos 2;
Servo servos[NumServos];
RH_RF95 LoRa(LoRa_CS,LoRa_INT);
HX711 LoadCell;

// Defining Telemetry Packet
struct TelemetryPacket {
  float loadCell;
  float pressures; // Just incremented numbers to print
  float pressure2; // Just incremented numbers to print
  float pressure3; // Just incremented numbers to print
};
// Initializing Command Packet
TelemetryPacket telemetry = {0.0,0.0,0.0,0.0};

// Defining Command Packet
struct CommandPacket {
  bool servo1State;
  bool servo2State;
};
// Initializing Command Packet
CommandPacket command = {0,0};

// Key Problem Variables
long LCoffset = 0;
float LCcalibrationFactor = 4299.18;
int servoPos[2] = [-1,-1];
bool servoAttached[2] = [false,false]
int open = 0;
int closed = 65;


// Main Setup
void setup() {
  
  // Opening Serial Monitor
  Serial.begin(9600);

  // Attaching Servos
  servo1.attach(Servo1_Pin);
  servoAttached[0] = true;
  servo2.attach(Servo2_Pin);
  servoAttached[0] = true;

  // Initializing LoadCell
  loadCell.begin(LoadCell_DT, LoadCell_CLK);
  while (!loadCell.is_ready()) {
    Serial.println("Waiting for HX711");
    delay(100);
  }
  delay(1000);
  Serial.println("Remove all load...");
  delay(3000);
  LCoffset = loadCell.read_average(20);
  Serial.print("Offset value: ");
  Serial.println(LCoffset);

  // Initializing LoRa
  pinMode(LoRa_RST, OUTPUT);
  digitalWrite(LoRa_RST, HIGH);
  delay(10);
  digitalWrite(LoRa_RST, LOW);
  delay(10);
  digitalWrite(LoRa_RST, HIGH);

  // Initializing SPI Protocol
  SPI.begin();

  // Edge Case that LoRa Initialization Failed
  if (!LoRa.init()) {
    Serial.println("LoRa Initialization Failed");
    while(1);
  }

  // Setting LoRa Parameters
  LoRa.setFrequency(LoRa_Freq);
  LoRa.setTxPower(23, false);

  // Printing System is Ready
  Serial.println("LoRa Transmitter Ready");
}

void loop() {
  
  readCommands();

  updateServos();

  readLoadCell();

  sendTelemetry();

}

****************************
// Functions
****************************

void readCommands()
{

  if (LoRa.available())
  {
    uint8_t buf[sizeof(CommandPacket)];
    uint8_t len = sizeof(buf);

    if (LoRa.recv(buf, &len))
    {
      // Checks against corrupted packets
      if (len == sizeof(CommandPacket))
      {
        // Copies packet into struct
        memcpy(&command, buf, sizeof(CommandPacket));

        Serial.print("Servo1 Command: ");
        Serial.println(command.servo1State);

        Serial.print("Servo2 Command: ");
        Serial.println(command.servo2State);
      }
    }
  }

}

void updateServos()
{
  int servo1Command = command.servo1State ? open : closed;
  int servo2Command = command.servo2State ? open : closed;
}



















