
unsigned long start = 0;
unsigned int channel = 1;

const char* mynetwork="mynetwork";  
const char* mypassword = "mypassword";

#include <ESP8266WiFi.h>
#include "./functions.h"

void setup() {
  Serial.begin(57600);

  wifi_set_opmode(STATION_MODE);            // Promiscuous works only with station mode
  wifi_set_channel(channel);
  wifi_promiscuous_enable(0);
  wifi_set_promiscuous_rx_cb(promisc_cb);   // Set up promiscuous callback
  wifi_promiscuous_enable(1);
  start=millis();
  LittleFS.format();
  Serial.println("Mount LittleFS");
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }
}

void loop() {
  channel = 1;
  wifi_set_channel(channel);
  while (true) {
    nothing_new++;                          // Array is not finite, check bounds and adjust if required
    if (nothing_new > 100) {
      nothing_new = 0;
      channel++;
      if (channel == 15) break;             // Only scan channels 1 to 14
      wifi_set_channel(channel);
    }
    delay(1);  // critical processing timeslice for NONOS SDK! No delay(0) yield()

    if(false) //comment out to enable reporting (cf. report-function in functions.h)
    if((millis()-start)>=10*60*1000) {
        report();
        start=millis();
    } 
  }
}
