#include <Arduino.h>
/////////////////////////////////////////////////////////////////////////////////////////////////
// Pneumatron Software - automated mode - v1 (04/2021)
// Parameters to be adjusted:

#define pneumatron 7

// 1- Calibration: Define slope and intercept from calibration:
float slope = 0.0183;
float intercept = 0.297;
// 2- Set time of gas discharge measurement, in seconds * 0.5 (e.g. 120= 60 seconds):
int duration = 120;
// 3- Set time interval (in minutes)
int interval = 15;
// 4- Set pressure range, in kPa
int PressureHigh = 50;
int PressureLow = 10;
//
/////////////////////////////////////////////////////////////////////////////////////////////////

int Pump = 6;
int Solenoid = 5;
long time_1 = 1;
long time_2 = 1;
long measure = 1;
unsigned long t_measure = 0;

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
Adafruit_ADS1115 ads;

float pressure = 0.0;

void setup() {
  Serial.begin(115200);
  ads.begin();
  ads.setGain(GAIN_EIGHT);

  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);

  t_measure = millis() + (interval * 60000);
}

void loop() {
  float pressure = ads.readADC_Differential_0_1();
  pressure = ((pressure) * (slope) + intercept);

  if (millis() - t_measure > interval * 60000) {
    t_measure = millis();
    {
      for (int i = 0; i < duration; i++) {
        float pressure = ads.readADC_Differential_0_1();
        pressure = ((pressure) * (slope) + intercept);

        if (pressure > PressureHigh) {
          digitalWrite(Pump, LOW);
          digitalWrite(Solenoid, LOW);
        } else if (pressure < PressureLow) {
          digitalWrite(Pump, HIGH);
          digitalWrite(Solenoid, HIGH);
        }
        Serial.print(pneumatron);
        Serial.print(',');
        Serial.print(time_1);
        Serial.print(',');
        Serial.print(measure);
        Serial.print(',');
        Serial.print(time_2);
        Serial.print(',');
        Serial.print(pressure, 5);
        Serial.println('\n');


        delay(500);
        time_1++;
        time_2++;
      }
    }
    digitalWrite(Pump, LOW);
    measure++;
    time_2 = 1;
  } else if (pressure > 5) {
    digitalWrite(Solenoid, HIGH);
    delay(1000);
    digitalWrite(Solenoid, LOW);
    delay(1000);
  }
}