// Code for Controlling a Servo Motor Remotely Receiver Side
// Contributors - Carson Mead

// Including Necessary Libraries
#include <RH_RF95.h>
#include <SPI.h>
#include <Servo.h>

// Defining LoRa and Switch Pins
#define LoRa_CS 10
#define LoRa_RST 9
#define LoRa_INT 2
#define LoRa_Freq 915.0
#define Servo_Pin 5

// Defining Servo States
int open = 0;
int closed = 65;
int lastCommand = -1;
bool servoAttached = false;
unsigned long servoMoveStart = 0;
const int servoMoveTime = 250; // ms

// Initializing radio and servo objects
RH_RF95 LoRa(LoRa_CS,LoRa_INT);
Servo servo1;

// Main Setup
void setup() {
  
  // Opening serial monitor
  Serial.begin(9600);

  // Attaching servo
  servo1.attach(Servo_Pin);

  // Initializing LoRa
  pinMode(LoRa_RST,OUTPUT);
  digitalWrite(LoRa_RST,HIGH);
  delay(100);
  digitalWrite(LoRa_RST,LOW);
  delay(10);
  digitalWrite(LoRa_RST,HIGH);
  delay(10);

  // Edge case that radio doesn't initialize
  if (!LoRa.init())
  {
    Serial.println("LoRa Initialization Failed");
    while (1);
  }

  // Radio Settings
  LoRa.setFrequency(LoRa_Freq);

  // Printing Ready to Serial Monitor
  Serial.println("LoRa Ready");

}

void loop() {
  
  if (LoRa.available())
  {
    uint8_t buf[1];
    uint8_t len = sizeof(buf);

    if (LoRa.recv(buf, &len))
    {
      uint8_t command = buf[0];

      Serial.print("Received: ");
      Serial.println(command);

      if (command != lastCommand)
      {
        lastCommand = command;

        if (command == 1)
        {
          moveServo(open);
        }
        else
        {
          moveServo(closed);
        }
      }
    }
  }

  if (servoAttached && millis() - servoMoveStart > servoMoveTime)
  {
    servo1.detach();
    servoAttached = false;
  }

}

void moveServo(int position) {

  if (!servoAttached)
  {
    servo1.attach(Servo_Pin);
    servoAttached = true;
  }

  servo1.write(position);
  servoMoveStart = millis();
}
