const int PRESSURE_DELAY = 500; // Time in ms between each pressure measurement during one measurement of gas discharge or vessel length distribuition

void performMeasurement(int &sequence, // increase at every pressure measurement
                        int &measure, // increase at every measurement of gas discharge or vessel length distribuition
                        int numPressureMeasures, // Number of pressure measurements during one measurement of gas discharge or vessel length distribuition
                        int pressureHigh, // keep pressure between pressureHigh and pressureLow
                        int pressureLow) {
  // local variables
  int group = random(9999); // random group to make each measurement unique
  int logLine = 1; // during one measurement of gas discharge or vessel length distribuition, increase at every pressure measurement, then it is reset
  float pressure;
  float temperature;
  float voltage;

  // during a measure only led2 is on
  digitalWrite(led1, LOW);
  digitalWrite(led2, HIGH);

  for(int i = 0; i < numPressureMeasures; i++) { // Read BMP280 for time set in duration
    // Read sensors
    pressure = bmp.readPressure() / 1000.0F; // convert pressure por kPa
    temperature = bmp.readTemperature();
    voltage = ESP.getVcc() / 1000.0;

    keepPressureInsideRange(pressure, pressureHigh, pressureLow); // Keep pressure range inside tubes
    sendData(sequence, logLine, measure, group, pressure, temperature, voltage); // Send sensors data
    
    sequence++;
    logLine++;
    delay(PRESSURE_DELAY);
  }
  
  measure++;
  digitalWrite(led2, LOW); // keep led2 LOW when not performing a measurement
}

void keepPressureInsideRange(int pressure, int pressureHigh, int pressureLow) { // Keep pressure between pressureHigh and pressureLow
  if (pressure < pressureLow) { 
    digitalWrite(pump, LOW);
    digitalWrite(solenoid, LOW);  
  } else if (pressure > pressureHigh) {
    digitalWrite(pump, HIGH);
    digitalWrite(solenoid, HIGH);
  }
}

void sendData(int sequence, int logLine, int measure, int group, float pressure, float temperature, float voltage) {
  data.sequence = sequence;
  data.logLine = logLine;
  data.measure = measure;
  data.group = group;
  data.pressure = pressure;
  data.temp = temperature;
  data.v = voltage;
      
  esp_now_send(0, (uint8_t *) &data, sizeof(data));
}
