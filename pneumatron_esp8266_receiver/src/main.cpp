#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <espnow.h>

// Structure example to receive data
// Must match the sender structure

// Set your new MAC Address
//uint8_t newMACAddress[] = {0x32, 0xAE, 0xA4, 0x07, 0x0D, 0x66};

typedef struct struct_message {
  int id;
  unsigned long ms;
  float temp1;
  float humid1;
  float pressure;
  float temp2;
  float humid2;
  float pressure2;
  long sequence;
  long measure;
  int log_line;
  float v;
} struct_message;

struct_message myData;

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  String dataString = "";
  dataString += String(myData.id);
  dataString += ",";
  dataString += String(myData.ms);
  dataString += ",";
  dataString += String(myData.temp1);
  dataString += ",";
  dataString += String(myData.pressure);
  dataString += ",";
  dataString += String(myData.humid1);  
  dataString += ",";
  dataString += String(myData.temp2);
  dataString += ",";
  dataString += String(myData.pressure2);
  dataString += ",";
  dataString += String(myData.humid2);  
  dataString += ",";
  dataString += String(myData.sequence);
  dataString += ",";
  dataString += String(myData.measure);
  dataString += ",";
  dataString += String(myData.log_line);
  dataString += ",";
  dataString += String(myData.v);
  dataString += "\n";
  Serial.println(dataString);
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  //wifi_set_macaddr(STATION_IF, &newMACAddress[0]);
  
  Serial.print("[NEW] ESP8266 Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
   esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
   esp_now_register_recv_cb(OnDataRecv);
}

void loop(){
}
