const int PRESSURE_MEASUREMENTS_GAS_DISCHARGE = 120; // Number of pressure measurements during one gas discharge measurement
const unsigned long MEASURE_INTERVAL_GAS_DISCHARGE = 15 * 60000; // Set time interval between each gas discharge measurement, in minutes * 60000
// Set pressure range inside discharge tube, in kPa
const int PRESSURE_HIGH_GAS_DISCHARGE = 85; // pressure_high// try to make this in a dynamic way
const int PRESSURE_LOW_GAS_DISCHARGE = 45; // pressure_low

// global variables
int sequenceGasDischarge = 1; // increase at every pressure measurement
int measureGasDischarge = 1; // increase at every gas discharge measurement
unsigned long lastGasDischargeMeasurementTime = 0;

void loopGasDischarge() {
  // local variables
  bool newMeasure = buttonCheckNewMeasure(); // Trigger a new gas discharge measurement if button 2 is pressed
  unsigned long currentTime = millis();
  unsigned long timeSinceLastMeasurement = currentTime - lastGasDischargeMeasurementTime;
  
  if (deviceMode != GAS_DISCHARGE) { // only continues if deviceMode is GAS_DISCHARGE
    data.version = "erro1";
    return;
  }
  ledFlashGasDischarge(); // Show that GAS_DISCHARGE mode is running

  if (timeSinceLastMeasurement >= MEASURE_INTERVAL_GAS_DISCHARGE || newMeasure) {
    performMeasurement(sequenceGasDischarge, measureGasDischarge, PRESSURE_MEASUREMENTS_GAS_DISCHARGE, PRESSURE_HIGH_GAS_DISCHARGE, PRESSURE_LOW_GAS_DISCHARGE);
    lastGasDischargeMeasurementTime = millis();
  }
  equilibratePressure();
}

void equilibratePressure() { // Equilibrate pressure with atmosfere
  float pressure;

  pressure = bmp.readPressure() / 1000.0F;
  if (pressure < PRESSURE_HIGH_GAS_DISCHARGE) { 
    digitalWrite(solenoid, HIGH);
    delay(1000);
    digitalWrite(solenoid, LOW);
  }
}
