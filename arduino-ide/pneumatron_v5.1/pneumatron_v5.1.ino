/*
  Pneumatron Sender Software - 03/2024 - v4
  by: Luciano Pereira & Gabriel Pires (gabriel.slpires@gmail.com)

  This code was developed for Pneumatron v5.1 and futher.

  Description:
  It can alternate between two modes, GAS_DISCHARGE and VESSEL_LENGTH, by
  pressing a button for 3s. Both modes performs a gas extraction measure with a
  vacuum pump and a pressure sensor. But they are slightly distinct, and used
  at different sets of experiments.

  Data comunication to master....

  Components:
  A BMP280 sensor with a pressure accuracy of 1.2 kPa.
  A generic vacuum pump capable of creating a partial vacuum of 40 kPa within the tubes.
  A generic solenoid valve...

  Modes:
   -GAS_DISCHARGE: for vulnerability curves
   -VESSEL_LENGTH: for vessel lenght distribution measurements

  
  Code available in: https://github.com/GabrielSlPires/pneumatron_devices
  Code developed in Arduino IDE 2.3.2 enviroment
*/

#include <Wire.h> // I2C communication
#include <Adafruit_BMP280.h> // BMP280 sensor
#include <ESP8266WiFi.h> // Wireless communication
#include <espnow.h> // Wireless communication

// constant variables
const int pneumatron = 106; // Pneumatron ID
uint8_t broadcastAddress[] = {0xE8, 0xDB, 0x84, 0xCC, 0xB0, 0x64}; // receiver MAC Address

// define peripherals 
const int pump = 14;       // D5 (wemos)
const int solenoid = 12;   // D6 (wemos)
const int btn1 = 13;       // D7 (wemos)
const int btn2 = 0;        // D3 (wemos)
const int led1 = 15;       // D8 (wemos)
const int led2 = 2;        // D4 (wemos)

  VESSEL_LENGTH,
};

enum ButtonState { // define button states to code easily
  UP,
  DOWN
};

struct { // data structure for wireless communication
  int id;
  int group;
  int sequence;
  int measure;
  int logLine;
  float humid;
  float pressure;
  float temp;
  float v;
  String version; // vou usar aqui para reportar algum erro
  //String version = "v3"; // se usar o novo jeito de fazer o ad, tem q ser v4
} data; // adicionar uma nova coluna, mode: ad ou vld

Adafruit_BMP280 bmp; // For BMP280

ADC_MODE(ADC_VCC);  // to measure voltage

// functions prototypes


void setup() {
  // initialize serial communication
  Serial.begin(115200);
  Serial.println("Starting Device...");

  // initialize digital I/O
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
  while (esp_now_init() != 0) {
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

  data.version = "v3"; // remover depois

  // Custom actions to finish setup
  data.id = pneumatron;
  data.humid = 0; // BMP280 do not measures humid
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
