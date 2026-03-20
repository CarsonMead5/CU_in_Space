#include <SPI.h>
#include <RF24.h>

RF24 radio(9,10);        // CE, CSN pins

byte address[6] = "NODE1";

int receivedValue = 0;

void setup() {

  Serial.begin(9600);
  delay(2000);

  Serial.println("Receiver starting...");

  if (!radio.begin()) {
    Serial.println("Radio not detected!");
    while (1);
  }

  // Force stable settings
  radio.setPALevel(RF24_PA_MIN);    // lowest power draw
  radio.setDataRate(RF24_250KBPS);  // more reliable
  radio.setChannel(76);
  radio.setAutoAck(false);

  radio.openReadingPipe(0, address);
  radio.startListening();

  Serial.println("Receiver ready and listening...");
}

void loop() {

  if (radio.available()) {

    radio.read(&receivedValue, sizeof(receivedValue));

    Serial.print("Received value: ");
    Serial.println(receivedValue);

  }

}
