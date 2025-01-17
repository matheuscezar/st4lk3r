
extern "C" {
#include "user_interface.h"
  typedef void (*freedom_outside_cb_t)(uint8 status);
  int  wifi_register_send_pkt_freedom_cb(freedom_outside_cb_t cb);
  void wifi_unregister_send_pkt_freedom_cb(void);
  int  wifi_send_pkt_freedom(uint8 *buf, int len, bool sys_seq);
}

#include <ESP8266WiFi.h>
#include "./structures.h"
#include <FS.h>
#include <LittleFS.h>

#define MAX_APS_TRACKED 100
#define MAX_CLIENTS_TRACKED 200

beaconinfo aps_known[MAX_APS_TRACKED];                    // Array to save MACs of known APs
int aps_known_count = 0;                                  // Number of known APs
int nothing_new = 0;
clientinfo clients_known[MAX_CLIENTS_TRACKED];            // Array to save MACs of known CLIENTs
int clients_known_count = 0;                              // Number of known CLIENTs

int register_beacon(beaconinfo beacon)
{
  int known = 0;   // Clear known flag
  for (int u = 0; u < aps_known_count; u++)
  {
    if (!memcmp(aps_known[u].bssid, beacon.bssid, ETH_MAC_LEN)) {
      known = 1;
      break;
    }   // AP known => Set known flag
  }
  if (! known & beacon.err == 0)  // AP is NEW (and not erroneous), copy MAC to array and return it
  {
    memcpy(&aps_known[aps_known_count], &beacon, sizeof(beacon));
    aps_known_count++;
    //Serial.printf("+1A;\r\n");
    if ((unsigned int) aps_known_count >=
        sizeof (aps_known) / sizeof (aps_known[0]) ) {
      Serial.printf("exceeded max aps_known\n");
      aps_known_count = 0;
    }
  }
  return known;
}

int register_client(clientinfo ci)
{
  int known = 0;   // Clear known flag
  for (int u = 0; u < clients_known_count; u++)
  {
    if (!memcmp(clients_known[u].station, ci.station, ETH_MAC_LEN)) {
      known = 1;
      break;
    }
  }
  if (! known)
  {
    memcpy(&clients_known[clients_known_count], &ci, sizeof(ci));
    clients_known_count++;
    //Serial.printf("+1C;\r\n");
    if ((unsigned int) clients_known_count >=
        sizeof (clients_known) / sizeof (clients_known[0]) ) {
      Serial.printf("exceeded max clients_known\n");
      clients_known_count = 0;
    }
  }
  return known;
}


void appendFile(const char *path, const char *message) {
  File file = LittleFS.open(path, "a");
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  file.print(message);
  file.close();
}

void readFile(const char *path) {
  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.println("Read from file: ");
  while (file.available()) { Serial.write(file.read()); }
  file.close();
}


void print_beacon(beaconinfo beacon)
{
  if (beacon.err != 0) {
    //Serial.printf("BEACON ERR: (%d)\r\n", beacon.err);
  } else {
    char macStr[60];
    sprintf(macStr, "[%32s] - %02x:%02x:%02x:%02x:%02x:%02x \n", beacon.ssid, beacon.bssid[0], beacon.bssid[1], beacon.bssid[2], beacon.bssid[3], beacon.bssid[4], beacon.bssid[5]);
    appendFile("history.txt", macStr);
    Serial.printf("BEACON: <==================== [%32s]  ", beacon.ssid);
    for (int i = 0; i < 5; i++) {
      Serial.printf("%02x:", beacon.bssid[i]);
    }
    Serial.println("");
    readFile("history.txt"); 
  }
}

void print_client(clientinfo ci)
{
  int u = 0;
  int known = 0;   // Clear known flag
  if (ci.err != 0) {
    Serial.printf("CLIENT ERR: (%d)\r\n", ci.err);
  } else {
    Serial.printf("DEVICE: ");
    for (int i = 0; i < 5; i++) Serial.printf("%02x:", ci.station[i]);
    Serial.printf("%02x", ci.station[5]);
    Serial.printf(" ==> ");

    for (u = 0; u < aps_known_count; u++)
    {
      if (!memcmp(aps_known[u].bssid, ci.bssid, ETH_MAC_LEN)) {
        Serial.printf("[%32s]", aps_known[u].ssid);
        known = 1;     // AP known => Set known flag
        break;
      }
    }
    
    if (! known)  {
      Serial.printf("!%32s!", "AP UNKNOWN");
      Serial.printf("%2s", " ");
      Serial.printf("%17s","  ???");
      Serial.printf("  ???");
      Serial.printf("   %4d", ci.rssi);      
    } else {
      Serial.printf("%2s", " ");
      for (int i = 0; i < 5; i++) Serial.printf("%02x:", ci.bssid[i]);
      Serial.printf("%02x", ci.bssid[5]);
      Serial.printf("  %3d", aps_known[u].channel);
      Serial.printf("   %4d", ci.rssi);
    }
  }
  Serial.printf(" %4d  %4d\r\n",aps_known_count,clients_known_count);

}

void promisc_cb(uint8_t *buf, uint16_t len)
{
  signed potencia;
  if (len == 12) {
    struct RxControl *sniffer = (struct RxControl*) buf;
    potencia = sniffer->rssi;
  } else if (len == 128) {
    struct sniffer_buf2 *sniffer = (struct sniffer_buf2*) buf;
    struct beaconinfo beacon = parse_beacon(sniffer->buf, 112, sniffer->rx_ctrl.rssi);
    potencia = sniffer->rx_ctrl.rssi;
    
    if (register_beacon(beacon) == 0) {
      print_beacon(beacon);
      nothing_new = 0;
    }    
    
  } else {
    struct sniffer_buf *sniffer = (struct sniffer_buf*) buf;
    potencia = sniffer->rx_ctrl.rssi;

    //Is data or QOS?
    if ((sniffer->buf[0] == 0x08) || (sniffer->buf[0] == 0x88)) {
      struct clientinfo ci = parse_data(sniffer->buf, 36, sniffer->rx_ctrl.rssi, sniffer->rx_ctrl.channel);
      if (memcmp(ci.bssid, ci.station, ETH_MAC_LEN)) {
        if (register_client(ci) == 0) {
          print_client(ci);
          nothing_new = 0;
        }
      }
    }
  }

  // Position 12 in the array is where the packet type number is located
  // For info on the different packet type numbers check:
  // https://stackoverflow.com/questions/12407145/interpreting-frame-control-bytes-in-802-11-wireshark-trace
  // https://supportforums.cisco.com/document/52391/80211-frames-starter-guide-learn-wireless-sniffer-traces
  // https://ilovewifi.blogspot.mx/2012/07/80211-frame-types.html
  if((buf[12]==0x88)||(buf[12]==0x40)||(buf[12]==0x94)||(buf[12]==0xa4)||(buf[12]==0xb4)||(buf[12]==0x08))
  {
    
    struct sniffer_buf *sniffer = (struct sniffer_buf*) buf;
    potencia = sniffer->rx_ctrl.rssi;
      struct clientinfo ci = parse_data(sniffer->buf, 36, sniffer->rx_ctrl.rssi, sniffer->rx_ctrl.channel);
      if (memcmp(ci.bssid, ci.station, ETH_MAC_LEN)) {
        if (register_client(ci) == 0) {
          print_client(ci);
          nothing_new = 0;
        }
      }
    
    // Enable the following lines if you want to scan for a specific MAC address.Specify desired MAC address on line 8 of structures.h
    /*int same = 1;
    for(int i=0;i<6;i++)
    {
      if(buf[22+i]!=desired[i])
      {
        same=0;
        break;
      }
    }
    if(same)
    {

    }
    //different device
    else
    {

    }*/
  }
  //Different packet type numbers
  else
  {

  }
}

void report()
{
  wifi_promiscuous_enable(0);
  Serial.printf("\r\nAfter %2d min it's time for a short report.\r\n",(millis()-start)/1000/60);
  if (WiFi.status() != WL_CONNECTED){
    WiFi.begin(mynetwork, mypassword);
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected, IP address is ");
    Serial.println(WiFi.localIP());
    Serial.println();
  }
  
  // WiFiClient espClient;
  // PubSubClient client(espClient);
  // client.setServer(MQTT_BROKER_IP, 1883);
  // client.publish("/home/data", "Hello World");
  // delay(500);
  // ESP.restart();
  
  wifi_promiscuous_enable(1);

}
