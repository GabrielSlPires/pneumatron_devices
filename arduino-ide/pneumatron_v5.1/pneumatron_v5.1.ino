/*
  Pneumatron Sender Software - 03/2024 - v4
  by: Luciano Pereira & Gabriel Pires (gabriel.slpires@gmail.com)

  This code was developed for Pneumatron v5.1 and futher.
  It can alternate between two mode, AIR_DISCHARGE and VESSEL_LENGTH, by pressing a button for 3s in the hardware

  AIR_DISCHARGE mode: Measures the amount of air discharged trought time with a BMP280 sensor and send the data to a receiver module.
  VESSEL_LENGTH mode: ...

  
  BMP280 has a pressure accuracy of 1.2 kPa

  Code available in: https://github.com/GabrielSlPires/pneumatron_devices
  Code developed in Arduino IDE 2.3.2 enviroment
*/

// For BMP280 sensor
#include <Adafruit_BMP280.h>
#include <Wire.h>

// For wireless communication
#include <ESP8266WiFi.h> 
#include <espnow.h>

const int pneumatron = 106; // Pneumatron ID

// receiver MAC Address
uint8_t broadcastAddress[] = {0xE8, 0xDB, 0x84, 0xCC, 0xB0, 0x64};

// define device modes
enum DeviceMode {
  AIR_DISCHARGE,
  VESSEL_LENGTH,
};
DeviceMode deviceMode = AIR_DISCHARGE; // Start with AIR_DISCHARGE

// create button states to program change_mode()
enum ButtonState {
  UP,
  DOWN
};

// define peripherals 
int pump = 14;       // D5 (wemos)
int solenoid = 12;   // D6 (wemos)
int btn1 = 13;       // D7 (wemos)
int btn2 = 0;        // D3 (wemos)
int led1 = 15;       // D8 (wemos)
int led2 = 2;        // D4 (wemos)

// For BMP280
float pressure; 
Adafruit_BMP280 bmp;

ADC_MODE(ADC_VCC);  // to measure voltage

// data structure for wireless communication
struct {
  int id;
  int group;
  int sequence;
  int measure;
  int logLine;
  float humid;
  float pressure;
  float temp;
  float v;
  String version = "v3"; // se usar o novo jeito de fazer o ad, tem q ser v4
} data; // adicionar uma nova coluna, mode: ad ou vld

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
  Serial.println("Starting Device...");

  if (!bmp.begin(0x76)) {
    Serial.println("Failed to initialize BMP280");
  }
  
  pinMode(pump, OUTPUT);
  pinMode(solenoid, OUTPUT);
  pinMode(btn1, INPUT_PULLUP); 
  pinMode(btn2, INPUT_PULLUP);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

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

  randomSeed(micros() + pneumatron); //Start random generator

  startUpFlash();
}

void loop() {
  check_button1_to_change_mode();
  switch (deviceMode) {
    case (AIR_DISCHARGE):
      loopAirDischarge();
      break;
    case (VESSEL_LENGTH):
      loopVesselLength();
      break;
  }
}
