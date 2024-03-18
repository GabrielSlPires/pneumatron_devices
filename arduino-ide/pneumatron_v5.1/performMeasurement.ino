const int PRESSURE_DELAY = 500; // Time in ms between each pressure measurement during one measurement of gas discharge or vessel length distribuition
const int PUMP_COUNTER_LIMIT = 2; // Maximum time the pressure pump is allowed to be on (2 = 2 * 500ms (PRESSURE_DELAY) = 1s)

void performMeasurement(int &sequence, // increase at every pressure measurement
                        int &measure, // increase at every measurement of gas discharge or vessel length distribuition
                        int numPressureMeasures, // Number of pressure measurements during one measurement of gas discharge or vessel length distribuition
                        float pressureHigh, // keep pressure between pressureHigh and pressureLow
                        float pressureLow) {
  // local variables
  int group = random(9999); // random group to make each measurement unique
  int logLine = 1; // during one measurement of gas discharge or vessel length distribuition, increase at every pressure measurement, then it is reset
  float pressure;
  float temperature;
  float voltage;
  bool pumpActivate = true; // at first pressure measure turns pump ON

  // during a measure only led2 is on
  digitalWrite(led1, LOW);
  digitalWrite(led2, HIGH);

  for(int i = 0; i < numPressureMeasures; i++) { // Read BMP280 for time set in duration
    // Read sensors
    pressure = bmp.readPressure() / 1000.0F; // convert pressure por kPa
    temperature = bmp.readTemperature();
    voltage = ESP.getVcc() / 1000.0;

    keepPressureInsideRange(pressure, pressureHigh, pressureLow, pumpActivate); // Keep pressure range inside tubes
    sendData(sequence, logLine, measure, group, pressure, temperature, voltage); // Send sensors data
    pumpActivate = false; // after first pressure measure turns pump ON only if pressure is out of range
    
    sequence++;
    logLine++;
    delay(PRESSURE_DELAY);
  }
  
  measure++;
  lastMeasurementTime = millis(); // Updated to avoid button triggers after measurements
  digitalWrite(led2, LOW); // keep led2 LOW when not performing a measurement
}

void keepPressureInsideRange(float pressure, float pressureHigh, float pressureLow, bool pumpActivate) { // Keep pressure between pressureHigh and pressureLow
  static bool pumpDeactivate = false;

  if (pressure < pressureLow || pumpDeactivate) {  // if pump was previous on, shut it down
    pumpDeactivate = false;
    digitalWrite(pump, LOW);
    digitalWrite(solenoid, LOW);  
  }
  if (pressure > pressureHigh || pumpActivate) { // at first pressure measure turns pump ON
    pumpDeactivate = true;
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
