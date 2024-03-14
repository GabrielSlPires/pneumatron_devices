const int PRESSURE_MEASUREMENTS_VESSEL_LENGTH = 300; // Number of pressure measurements during one vessel length measurement
// Set pressure range inside discharge tube, in kPa
const float PRESSURE_HIGH_VESSEL_LENGTH = 55; // pressure_high// try to make this in a dynamic way
const float PRESSURE_LOW_VESSEL_LENGTH = 45; // pressure_low

// global variables
int sequenceVesselLength = 1; // increase at every pressure measurement
int measureVesselLength = 1; // increase at every vessel length measurement

void loopVesselLength() {
  // local variables
  bool newMeasure = buttonCheckNewMeasure(); // Trigger a new vessel length measurement if button 2 is pressed
  
  if (deviceMode != VESSEL_LENGTH) { // only continues if deviceMode is GAS_DISCHARGE
    data.version = "erro2";
    return;
  }
  ledFlashVesselLength(); // Show that GAS_DISCHARGE mode is running

  if (newMeasure) {
    performMeasurement(sequenceVesselLength, measureVesselLength, PRESSURE_MEASUREMENTS_VESSEL_LENGTH, PRESSURE_HIGH_VESSEL_LENGTH, PRESSURE_LOW_VESSEL_LENGTH);
  }
}