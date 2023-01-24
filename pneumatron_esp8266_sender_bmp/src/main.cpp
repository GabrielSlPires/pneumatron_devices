
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Pneumatron-NANO-SENDER Software - (03/2022)
// Parameters to be adjusted:
#define pneumatron 13
// 2- Set time of gas discharge measurement, in seconds * 0.5 (e.g. 120= 60 seconds):
int duration = 120;
// 3- Set time interval (in minutes)
int interval = 15;
// 3- Set pressure range, in kPa
int PressureHigh = 400;
int PressureLow = 850;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int Pump = 14;      // D5 (wemos)
int Solenoid = 12;  // D6 (wemos)
long sequence = 1;
int log_line = 1;
long measure = 1;
unsigned long t_measure = 0;
unsigned long ms = 0;
float temp1, humid1, pressure, temp2, humid2, pressure2;

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_BMP280.h> // FOR BMP SENSOR!!!!
//Adafruit_BME280 bme1; // USING BME280
Adafruit_BME280 bme2;
Adafruit_BMP280 bme1;  // USING BMP280

#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>

ADC_MODE(ADC_VCC);  // to measure voltage

// REPLACE WITH RECEIVER MAC Address
// RECEIVER TEST SENSOR: 
// uint8_t broadcastAddress[] = {0x44, 0x17, 0x93, 0x0E, 0x48, 0xEE} ; // broken
// receiver 1 (ESP-12F) - Talking garden 
// uint8_t broadcastAddress[] = {0x44, 0x17, 0x93, 0x0F, 0x4B, 0xE9};
// receiver 1 (ESP-12F) - Talking garden 
// uint8_t broadcastAddress[] = {0x44, 0x17, 0x93, 0x0E, 0x53, 0xB0};
// receiver 2 (ESP-12F)
// uint8_t broadcastAddress[] = {0xE8, 0x9F, 0x6D, 0x93, 0xF4, 0x70};
// receiver 3 (ESP-12F)
// uint8_t broadcastAddress[] = {0xAC, 0x0B, 0xFB, 0xD9, 0xE5, 0xB2};
// receiver 4 (ESP-12F)
// uint8_t broadcastAddress[] = {0x44, 0x17, 0x93, 0x0f, 0xB4, 0xE9};
// receiver 5 (ESP-12F)
// uint8_t broadcastAddress[] = {0xAC, 0x0B, 0xFB, 0xDA, 0x2F, 0x51};
// receiver 6 (ESP-12F)
// uint8_t broadcastAddress[] = {0xE8, 0x9F, 0x6D, 0x93, 0xD4, 0x61};


// receiver UNICAMP (ESP-12F)
 uint8_t broadcastAddress[] = {0xBC, 0xFF, 0x4D, 0xF7, 0xF7, 0x4C};

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

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
/*  Serial.print("\r\nLast Packet Send Status: ");
  if (sendStatus == 0){
//    Serial.println("Delivery success");
  }
  else{
//    Serial.println("Delivery fail");
  }
*/  
}

void setup()
{
  Serial.begin(115200);
  digitalWrite(D2, LOW); // this is not the same as pinmode!
  digitalWrite(D1, LOW);
  pinMode(D2, INPUT_PULLUP);
  pinMode(D1, INPUT_PULLUP);
  Wire.begin(D2, D1);
// Wire.setClock(100000); // Set I2C bus speed
  bme1.begin(0x76);
  bme2.begin(0x77);
  
  pinMode(14, OUTPUT); //solenoide/bomba
  pinMode(12, OUTPUT); //solenoide/bomba

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
  
  myData.sequence = 1;
  myData.measure = 1;
  myData.log_line = 1;
  t_measure = millis()+(interval*60000);
}

void loop()
{ 
    pressure = bme1.readPressure() / 100.0F;
    
  if (millis() - t_measure > interval * 60000) //
    {
      {
      t_measure = millis();
      for(int i = 0; i < duration; i++)
              {         
  myData.pressure = bme1.readPressure() / 100.0F; 
 
    if (myData.pressure < PressureHigh)
    {
      digitalWrite(Pump, LOW);
      digitalWrite(Solenoid, LOW);  
    }
    else if (myData.pressure > PressureLow)
    {
      digitalWrite(Pump, HIGH);
      digitalWrite(Solenoid, HIGH);
    }

  myData.temp1 = bme1.readTemperature();
//  myData.humid1 = bme1.readHumidity();

  myData.temp2 = bme2.readTemperature();
  myData.humid2 = bme2.readHumidity();
  myData.pressure2 = bme2.readPressure() / 10.0F;
  myData.v  = ESP.getVcc() / 1000.0;

  myData.ms = millis();
  myData.id = pneumatron;
 
  esp_now_send(0, (uint8_t *) &myData, sizeof(myData));

  delay(500);
  myData.sequence++;
  myData.log_line++;
              }
  digitalWrite(Pump, LOW);
  digitalWrite(Solenoid, LOW);
 
  myData.measure++;
  myData.log_line = 1;
       }
   }
  else if (pressure <900){
  digitalWrite(Solenoid, HIGH);
  delay(1000);
  digitalWrite(Solenoid, LOW);
  delay(1000);
  }
 
}
