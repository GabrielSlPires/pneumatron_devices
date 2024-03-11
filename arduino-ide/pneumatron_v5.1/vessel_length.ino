const int N_PRESSURE_VLD = 300; // Number of pressure measurements during one vessel length distribution measurement
const int DELAY_PRESSURE_VLD = 500; // Time in ms between each pressure measurement during one vessel length distribution measurement
// Set pressure range inside discharge tube, in kPa
const int PRESSURE_HIGH_VLD = 55; // pressure_high// try to make this in a dynamic way
const int PRESSURE_LOW_VLD = 45; // pressure_low

int sequenceVld = 1; // increase at every pressure measurement
int logLineVld = 1; // during one vessel length distribution measurement, increase at every pressure measurement, then it is reset
int measureVld = 1; // increase at every vessel length distribution measurement

void loopVesselLength() {
  digitalWrite(led1, HIGH); // To show a different mode is running
  
  // Triggers a new vessel length measure when pressing button2
  ButtonState btn2_state = digitalRead(btn2) == HIGH ? UP : DOWN;
  if (btn2_state == DOWN) {
    digitalWrite(led2, HIGH); // during a measure led2 is on

    data.group = random(9999); // Create a random group to separete measures
    data.measure = measureVld;

    for(int i = 0; i < N_PRESSURE_VLD; i++) {
      createPartialVacuum(); // Keep pressure between PRESSURE_HIGH and PRESSURE_LOW
      sendSensorsData();
      
      sequenceVld++;
      logLineVld++;
      delay(DELAY_PRESSURE_VLD);
    }

    digitalWrite(pump, LOW);
    digitalWrite(solenoid, LOW);
    
    measureVld++;
    logLineVld = 1;   
  }
  digitalWrite(led2, LOW); // keep led2 LOW when not performing a measurement
}

void createPartialVacuum() { // If pressure is higher PRESSURE_HIGH, turns vacuum pump ON until PRESSURE_LOW is reached
  float vacuumPressure = bmp.readPressure() / 1000.0F;
  
  if (vacuumPressure > PRESSURE_HIGH_VLD) {
    while (vacuumPressure > PRESSURE_LOW_VLD) { // Keep pressure lower than 
      vacuumPressure = bmp.readPressure() / 1000.0F;
      digitalWrite(pump, HIGH);
      digitalWrite(solenoid, HIGH);
    }
  }
  digitalWrite(pump, LOW);
  digitalWrite(solenoid, LOW);
}

void sendSensorsData() { 
  data.humid = 0; // BMP280 do not measures humid
  data.pressure = bmp.readPressure() / 1000.0F; // Na analise de dados filtrar valores maiores que pressure high
  data.temp = bmp.readTemperature();
  data.v  = ESP.getVcc() / 1000.0;
  data.sequence = sequenceVld;
  data.logLine = logLineVld;

  esp_now_send(0, (uint8_t *) &data, sizeof(data));
}