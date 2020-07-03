// rf95_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf95_server
// Tested with Anarduino MiniWirelessLoRa, Rocket Scream Mini Ultra Pro with
// the RFM95W, Adafruit Feather M0 with RFM95

#include <SPI.h>
#include <RH_RF95.h>
#include "DHT.h"

// Singleton instance of the radio driver
RH_RF95 rf95;
#define DHTPIN 5
#define DHTTYPE DHT11
//RH_RF95 rf95(5, 2); // Rocket Scream Mini Ultra Pro with the RFM95W
//RH_RF95 rf95(8, 3); // Adafruit Feather M0 with RFM95 

// Need this on Arduino Zero with SerialUSB port (eg RocketScream Mini Ultra Pro)
//#define Serial SerialUSB
DHT dht(DHTPIN, DHTTYPE);
uint8_t humidity[6];
uint8_t temp[6];
uint8_t battery[6];
uint8_t trimbattery[2];
void setup() 
{
  // Rocket Scream Mini Ultra Pro with the RFM95W only:
  // Ensure serial flash is not interfering with radio communication on SPI bus
//  pinMode(4, OUTPUT);
//  digitalWrite(4, HIGH);

  Serial.begin(9600);
  dht.begin();
  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
//rf95.setTxPower(23, false);
  // If you are using Modtronix inAir4 or inAir9,or any other module which uses the
  // transmitter RFO pins and not the PA_BOOST pins
  // then you can configure the power transmitter power for -1 to 14 dBm and with useRFO true. 
  // Failure to do that will result in extremely low transmit powers.
//  driver.setTxPower(14, true);
}

void loop()
{
  Serial.println("Sending to rf95_server");
  // Send a message to rf95_server
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  Serial.println(h);
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  uint8_t id[] = "1234";
  int value = analogRead(0);
  float voltage = value * (5.0 / 1023.00) * 2;
  Serial.println(voltage);
  int v = ((voltage-5.5)/3)*100;
  Serial.println(v);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  rf95.send(id , sizeof(id));
  rf95.waitPacketSent();
  delay(100);
  dtostrf(h, 2, 0, humidity);
  rf95.send(humidity , sizeof(humidity));
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  delay(100);
 
  dtostrf(t, 2, 2, temp);
  //temp[5] = 49;
  rf95.send(temp , sizeof(temp));
  rf95.waitPacketSent();
  delay(100);
  
  dtostrf(v, 2, 0, battery);
  battery[2] = 118;
  rf95.send(battery , sizeof(battery));
  rf95.waitPacketSent();
  delay(10000);
}
