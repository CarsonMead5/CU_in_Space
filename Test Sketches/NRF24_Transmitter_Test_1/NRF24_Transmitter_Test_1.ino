#include <SPI.h>
#include <RF24.h>

RF24 radio(9,10);

byte address[6] = "NODE1";

int sendValue = 0;

void setup() {

  Serial.begin(9600);
  delay(2000);

  Serial.println("Transmitter starting...");

  if (!radio.begin()) {
    Serial.println("Radio not detected!");
    while (1);
  }

  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(76);
  radio.setAutoAck(false);

  radio.openWritingPipe(address);
  radio.stopListening();

  Serial.println("Transmitter ready...");
}

void loop() {

  sendValue++;

  radio.write(&sendValue, sizeof(sendValue));

  Serial.print("Sent value: ");
  Serial.println(sendValue);

  delay(1000);
}
