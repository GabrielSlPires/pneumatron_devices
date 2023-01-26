/*
  Pneumatron Sender Software - 01/2023 - v4
  by: Luciano Pereira & Gabriel Pires (gabriel.slpires@gmail.com)

  This code receives data from several Pneumatrons

  Code available in: https://github.com/GabrielSlPires/pneumatron_devices
  Code developed in PlatformIO enviroment
*/

#include "Arduino.h" // For PlatformIO

// For wireless communication
#include <ESP8266WiFi.h> 
#include <espnow.h>

// data structure for wireless communication
struct {
  int id;
  int group;
  int sequence;
  int measure;
  int log_line;
  float humid;
  float pressure;
  float temp;
  float v;
  String version;
} data;

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  // Store received incomingData in data
  memcpy(&data, incomingData, sizeof(data));
  // Create message print in serial output
  String message_output;

  message_output = String(data.id);
  message_output += ",";
  message_output += String(data.group);
  message_output += ",";
  message_output += String(data.sequence);
  message_output += ",";
  message_output += String(data.measure);
  message_output += ",";
  message_output += String(data.log_line);  
  message_output += ",";
  message_output += String(data.humid);
  message_output += ",";
  message_output += String(data.pressure);
  message_output += ",";
  message_output += String(data.temp);
  message_output += ",";
  message_output += String(data.v);  
  message_output += ",";
  message_output += String(data.version);
  message_output += "\n";

  Serial.println(message_output);
}
 
void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA); // Set device as a Wi-Fi Station

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
   esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
   esp_now_register_recv_cb(OnDataRecv);
}

void loop(){
}
