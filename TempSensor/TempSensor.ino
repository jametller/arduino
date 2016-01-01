//#include <Narcoleptic.h>
#include <SPI.h>
#include <string.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include "DHT.h"

#define DHTPIN 5     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

RF24 radio(9,10);
DHT dht(DHTPIN, DHTTYPE);

byte addresses[][6] = {"1Node","2Node"};

void setup() {

  Serial.begin(57600);
  printf_begin();

  // Setup and configure rf radio
  radio.begin();                          // Start up the radio
  radio.setAutoAck(1);                    // Ensure autoACK is enabled
  radio.setRetries(15,15);                // Max delay between retries & number of retries
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);
  radio.setDataRate(RF24_250KBPS);
  
  radio.startListening();                 // Start listening
  radio.printDetails();  // Dump the configuration of the rf unit for debugging
    
  dht.begin ();
}

void loop(void){
    char buffer[20];
    memset (buffer, 0, 20);
    char hum_f[7], tem_f[7];
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    Serial.println (t);
    Serial.println (h);
    dtostrf(h, 3, 2, buffer);
    strcat (buffer, "|");
    dtostrf(t, 3, 2, buffer + strlen(buffer));
    printf ("Data: %s\n\r", buffer);

    radio.stopListening();                                    // First, stop listening so we can talk.
    
    printf("Now sending \n\r");

    if (!radio.write(buffer, strlen(buffer))){  printf("failed.\n\r");  }
        
    radio.startListening();                                    // Now, continue listening

    // Try again 1s later
    printf ("Sleeping\n\r");
    Serial.flush();
    radio.powerDown();
  //  Narcoleptic.delay (3000);
    delay (300000);
    printf ("Out of sleep\n\r");
    radio.powerUp();
}

