const int N_PRESSURE = 120; // Number of pressure measurements during one gas discharge measurement
const int DELAY_PRESSURE = 500; // Time in ms between each pressure measurement during one gas discharge measurement
const unsigned long INTERVAL = 15 * 60000; // interval // Set time interval between each gas discharge measurement, in minutes * 60000
// Set pressure range inside discharge tube, in kPa
const int PRESSURE_HIGH = 85; // pressure_high// try to make this in a dynamic way
const int PRESSURE_LOW = 45; // pressure_low

int sequence = 1; // increase at every pressure measurement
int logLine = 1; // during one air discharge measurement, increase at every pressure measurement, then it is reset
int measure = 1; // increase at every gas discharge measurement

unsigned long lastMeasurementTime = 0;

void loopAirDischarge() {
  unsigned long currentTime = millis();
  FlashAD(); // To show a different mode is running
  pressure = bmp.readPressure() / 1000.0F; // convert pressure por kPa


  // Triggers a new gas discharge measure when pressing button2
  ButtonState btn2_state = digitalRead(btn2) == HIGH ? UP : DOWN;
  if (btn2_state == DOWN) {
    currentTime = currentTime + INTERVAL;
  }

  if (currentTime - lastMeasurementTime >= INTERVAL) {
    lastMeasurementTime = millis();

    digitalWrite(led1, LOW);
    digitalWrite(led2, HIGH); // during a measure only led2 is on

    data.group = random(9999); // Create a random group to separete measures
    data.measure = measure;

    for(int i = 0; i < N_PRESSURE; i++) { // Read BMP280 for time set in duration
      
      data.humid = 0; // BMP280 do not measures humid
      data.pressure = bmp.readPressure() / 1000.0F; // convert pressure por kPa
      data.temp = bmp.readTemperature();

      if (data.pressure < PRESSURE_LOW) { // Keep pressure between PRESSURE_HIGH and PRESSURE_LOW
        digitalWrite(pump, LOW);
        digitalWrite(solenoid, LOW);  
      } else if (data.pressure > PRESSURE_HIGH) {
        digitalWrite(pump, HIGH);
        digitalWrite(solenoid, HIGH);
      }
      
      data.v = ESP.getVcc() / 1000.0;
      data.sequence = sequence;
      data.logLine = logLine;
      
      esp_now_send(0, (uint8_t *) &data, sizeof(data)); // if error, og to lost conection loop?
      
      sequence++;
      logLine++;
      delay(DELAY_PRESSURE);
    }

    digitalWrite(pump, LOW);
    digitalWrite(solenoid, LOW);
    
    measure++;
    logLine = 1;   
    digitalWrite(led2, LOW); // keep led2 LOW when not performing a gas discharge measurement
  }

  if (pressure < PRESSURE_HIGH) { // Equilibrate pressure with atmosfere
    digitalWrite(solenoid, HIGH);
    delay(1000);
    digitalWrite(solenoid, LOW);
  }

}




/*
// vamos precisar testar para ver se não vai afetar a medida, pq primeiro vou da o vacuo e dps salvar os dados
void loopAirDischargeUpdated() {
  FlashAD();
  pressure = bmp.readPressure() / 1000.0F;
  
  time_diff = millis() - t_measure;
  if (time_diff > interval) { // Is TRUE every 15 minutes (time from interval)
    digitalWrite(led2, HIGH);

    data.group = random(9999); // Create a random group to separete measures
    data.measure = measure;

    // send pressure values before applying vacuum
    logLine = 0;
    sendSensorsData();

    createPartialVacuum();
    for(int i = 0; i < duration; i++) { // Read BMP280 for time set in duration
      sendSensorsData();
      sequence++;
      logLine++;
      delay(500);
    }
    
    measure++;
    logLine = 1;   
    t_measure = millis();
    
    digitalWrite(led2, LOW);

  } else if (pressure < pressure_high) { // Equilibrate pressure with atmosfere
    digitalWrite(solenoid, HIGH);
    delay(1000);
    digitalWrite(solenoid, LOW);
    delay(1000);
  }
}

void createPartialVacuum() { // Turns the vacuum pump ON until pressure_low is reached, pump is ON for 1s maximum
  unsigned long startVacuum = millis();
  float vacuumPressure = bmp.readPressure() / 1000.0F;
  float atmPressure = bmp.readPressure() / 1000.0F;
  
  while (millis() - startVacuum < 1000 && vacuumPressure < atmPressure * 0.45) { // Loop for 1s maximum, while pressure is higher than 40% of atmosferic pressure
    vacuumPressure = bmp.readPressure() / 1000.0F;
    digitalWrite(pump, HIGH);
    digitalWrite(solenoid, HIGH);
  }
  digitalWrite(pump, LOW);
  digitalWrite(solenoid, LOW);  
}

void sendSensorsData() { 
  data.humid = 0; // BMP280 do not measures humid
  data.pressure = bmp.readPressure() / 1000.0F; // Na analise de dados filtrar valores maiores que pressure high
  data.temp = bmp.readTemperature();
  data.v  = ESP.getVcc() / 1000.0;
  data.sequence = sequence;
  data.logLine = logLine;

  esp_now_send(0, (uint8_t *) &data, sizeof(data));
}
*/