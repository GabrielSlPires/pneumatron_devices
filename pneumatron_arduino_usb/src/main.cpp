/*
  Pneumatron Software - 01/2023 - v4
  by: Luciano Pereira & Gabriel Pires (gabriel.slpires@gmail.com)

  This code measures the amount of air discharged trought time with a pressure sensor and send the data to a computer by USB.

  Code available in: https://github.com/GabrielSlPires/pneumatron_devices
  Code developed in PlatformIO enviroment
*/

#include "Arduino.h" // For PlatformIO

// For pressure sensor
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Parameters to be adjusted:

#define pneumatron 4 // Pneumatron ID

// Calibration: Define slope and intercept from calibration:
float slope = 0.0183;
float intercept = 0.297;

int duration = 120; // Set time of gas discharge measurement, in seconds * 0.5 (e.g. 120 = 60 seconds):
unsigned long interval = 15; // Set time interval (in minutes)

// Set pressure range inside discharge tube, in kPa
int pressure_high = 85;
int pressure_low = 40;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int pump = 6;
int solenoid = 5;
int group;
int sequence = 1;
int log_line = 1;
int measure = 1;
unsigned long t_measure = 0;
unsigned long time_diff = 0;
String message_output;

// For pressure sensor
float pressure;
Adafruit_ADS1115 ads;

void setup() {
  Serial.begin(115200);

  ads.begin();
  ads.setGain(GAIN_EIGHT);

  pinMode(pump, OUTPUT);
  pinMode(solenoid, OUTPUT);

  interval = interval * 60000;
  t_measure = millis() + (interval);

  randomSeed(micros() + pneumatron); //Start random generator
}

void loop() {
  pressure = ads.readADC_Differential_0_1();
  pressure = 101.325 - ((pressure) * (slope) + intercept); // Convert output to absolute pressure 
  
  time_diff = millis() - t_measure;
  if (time_diff > interval) { // TRUE after interval (in minutes)
    t_measure = millis();
    group = random(9999); // Create a random group to separete measures

    for (int i = 0; i < duration; i++) { // Read pressure for time set in duration
      pressure = ads.readADC_Differential_0_1();
      pressure = 101.325 - ((pressure) * (slope) + intercept); // Convert output to absolute pressure 

      if (pressure < pressure_low) { // Keep pressure between pressure_high and pressure_low
        digitalWrite(pump, LOW);
        digitalWrite(solenoid, LOW);  
      } else if (pressure > pressure_high) {
        digitalWrite(pump, HIGH);
        digitalWrite(solenoid, HIGH);
      }
      
      // Create message print in serial output
      message_output = String(pneumatron);
      message_output += ",";
      message_output += String(group);
      message_output += ",";
      message_output += String(sequence);
      message_output += ",";
      message_output += String(measure);
      message_output += ",";
      message_output += String(log_line);
      message_output += ",";
      message_output += String(""); // Humid
      message_output += ",";
      message_output += String(pressure);
      message_output += ",";
      message_output += String(""); // Temperature
      message_output += ",";
      message_output += String(""); // Voltage 
      message_output += ",";
      message_output += String("v1"); // Version
      message_output += "\n";

      Serial.println(message_output);

      sequence++;
      log_line++;
      delay(500);
    }

    digitalWrite(pump, LOW);

    measure++;
    log_line = 1;

  } else if (pressure < pressure_high) { // Equilibrate pressure with atmosfere
    digitalWrite(solenoid, HIGH);
    delay(1000);
    digitalWrite(solenoid, LOW);
    delay(1000);
  }
}