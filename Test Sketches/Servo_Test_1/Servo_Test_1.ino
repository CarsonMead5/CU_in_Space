#include <Servo.h>

// Initializing the servo objects
Servo servo1;
Servo servo2;

// Declaring unsigned 8 bit constant integers
// Declaring servo signal pins
const uint8_t SERVO1_PIN = 9;
const uint8_t SERVO2_PIN = 10;

void setup() {
  // Initializing serial communication
  // 115200 is the baud rate (bits/second)
  Serial.begin(115200);

  // Attaching servo objects to their signal pins
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);

  Serial.println("Mega Dual Servo Test Ready");
}

void loop() {

  // Sweeping each servo from 0 to 180 degrees together
  for (int angle = 0; angle <= 180; angle += 1){
    servo1.write(angle);
    servo2.write(angle);
    delay(10);
  }

  // Delay for 500 ms
  delay(500);

  // Sweeping each servo back down to 0 degrees
  for (int angle = 180; angle >= 0; angle -= 1){
    servo1.write(angle);
    servo2.write(angle);
    delay(10);
  }

  // Delaying for 500 seconds to allow for debugging
  delay(30000);

}
