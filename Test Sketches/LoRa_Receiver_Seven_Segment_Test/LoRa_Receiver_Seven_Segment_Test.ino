// LoRa Receiver Test w/ Seven Segment Display
#include <SPI.h>
#include <RH_RF95.h>
#include <LedControl.h>

// Establishing radio ports and variables
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define RF95_FREQ 915.0

// DIN, CLK, CS, number of MAX7219 chips
LedControl lc = LedControl(23,24,25,1);

// Establishing radio object
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Initializing struct to be sent
struct packet{
  float value1;
  float value2;
}

// Diplay number function
void displayNumber(long number){

    for (int i=0; i<8; i++)
    {
      int digit = number%10;
      bool decimal = (i==2);
   
      lc.setDigit(0,i,digit,decimal);
      number /= 10;

      if (number == 0) break;
    }
}

void setup() {
  
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST,HIGH);

  Serial.begin(9600);
  delay(1000);

  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  SPI.begin();

  if(!rf95.init()) {
    Serial.println("LoRa Initialization Failed");
    while(1);
  }

  rf95.setFrequency(RF95_FREQ);

  // Waking up LED display
  lc.shutdown(0,false);

  // Brightness (0-15)
  lc.setIntensity(0,5);
  lc.setScanLimit(0,7);
  lc.clearDisplay(0);

}

void loop() {
  static float lastValue = 0;
  
  if (rf95.available())
  {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len))
    {
       float receivedValue;
       memcpy(&receivedValue, buf, sizeof(receivedValue));
       
       lastValue = receivedValue;

       Serial.print("Received: ");
       Serial.println(receivedValue);
    }    
  }

  long displayVal = (long)(lastValue*100);
        
  displayNumber(1);

  delay(10);
}
