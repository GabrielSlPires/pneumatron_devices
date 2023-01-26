/*
  Pneumatron Sender Software - 01/2023 - v4
  by: Luciano Pereira & Gabriel Pires (gabriel.slpires@gmail.com)

  This code measures the amount of air discharged trought time with a BMP280 sensor and send the data to a receiver module.
  BMP280 has a pressure accuracy of 1.2 kPa


  Code available in: https://github.com/GabrielSlPires/pneumatron_devices
  Code developed in PlatformIO enviroment
*/

#include "Arduino.h" // For PlatformIO

// For BMP280 sensor
#include <Adafruit_BMP280.h>
#include <Wire.h>

// For wireless communication
#include <ESP8266WiFi.h> 
#include <espnow.h>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Parameters to be adjusted:

#define pneumatron 31 // Pneumatron ID

int duration = 120; // Set time of gas discharge measurement, in seconds * 0.5 (e.g. 120 = 60 seconds):
unsigned long interval = 15; // Set time interval (in minutes)
// Set pressure range inside discharge tube, in kPa
int pressure_high = 85;
int pressure_low = 40;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Initialize variables

int pump = 14;      // D5 (wemos)
int solenoid = 12;  // D6 (wemos)
int sequence = 1;
int log_line = 1;
int measure = 1;
unsigned long t_measure = 0;
unsigned long time_diff = 0;

// For BMP280
float pressure; 
Adafruit_BMP280 bmp;

ADC_MODE(ADC_VCC);  // to measure voltage

// receiver MAC Address - UNICAMP (ESP-12F)
uint8_t broadcastAddress[] = {0xBC, 0xFF, 0x4D, 0xF7, 0xF7, 0x4C};

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
  String version = "v3";
} data;

// Identify communication problems
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
Serial.print("\r\nLast Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
    // add red LED
  }
}

void setup() {
  Serial.begin(115200);

  if (!bmp.begin(0x76)) {
    Serial.println("Failed to initialize BMP280");
  }
  
  pinMode(pump, OUTPUT);
  pinMode(solenoid, OUTPUT);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
 
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } 

  // Set ESP-NOW role
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  // Once ESPNow is successfully init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  data.id = pneumatron;

  interval = interval * 60000;
  t_measure = millis() + (interval);
}

void loop() {
  pressure = bmp.readPressure() / 1000.0F;
  
  time_diff = millis() - t_measure;
  if (time_diff > interval) { // TRUE after interval (in minutes)
    t_measure = millis();

    data.group = random(9999); // Create a random group to separete measures
    data.measure = measure;

    for(int i = 0; i < duration; i++) { // Read BMP280 for time set in duration
      
      data.humid = ""; // BMP280 do not measures humid
      data.pressure = bmp.readPressure() / 1000.0F;
      data.temp = bmp.readTemperature();

      if (data.pressure < pressure_low) { // Keep pressure between pressure_high and pressure_low
        digitalWrite(pump, LOW);
        digitalWrite(solenoid, LOW);  
      } else if (data.pressure > pressure_high) {
        digitalWrite(pump, HIGH);
        digitalWrite(solenoid, HIGH);
      }
      
      data.v  = ESP.getVcc() / 1000.0;
      data.sequence = sequence;
      data.log_line = log_line;
      
      esp_now_send(0, (uint8_t *) &data, sizeof(data));
      
      sequence++;
      log_line++;
      delay(500);
    }

    digitalWrite(pump, LOW);
    digitalWrite(solenoid, LOW);
    
    measure++;
    log_line = 1;   

  } else if (pressure < pressure_high) { // Equilibrate pressure with atmosfere
    digitalWrite(solenoid, HIGH);
    delay(1000);
    digitalWrite(solenoid, LOW);
    delay(1000);
  }
}
