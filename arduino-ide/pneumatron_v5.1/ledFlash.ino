const unsigned long INTERVAL_GAS_DISCHARGE = 1000;  // This is our Flash Interval for GAS_DISCHARGE (AD) mode
const unsigned long INTERVAL_VESSEL_LENGTH = 250;   // This is our Flash Interval for GAS_DISCHARGE (AD) mode
const unsigned long INTERVAL_DISCONNECTED = 100;    // This will flash led2 every 100ms with meassage delivery failed

unsigned long nextFlashGD;
unsigned long nextFlashVLD;
unsigned long nextFlashDisconnected;

void ledFlash(int ledPin, unsigned long FLASH_INTERVAL, unsigned long &nextFlash) {
  if (millis() < nextFlash) {
    return;
  }
  digitalWrite(ledPin, !digitalRead(ledPin));  // Toggle the LED State
  nextFlash = millis() + FLASH_INTERVAL;       // Sets the time it should toggle again.
}

void ledFlashGasDischarge() {
  ledFlash(led1, INTERVAL_GAS_DISCHARGE, nextFlashGD);
}

void ledFlashVesselLength() {
  ledFlash(led1, INTERVAL_VESSEL_LENGTH, nextFlashVLD);
}

void ledFlashDisconnected() {
  ledFlash(led1, INTERVAL_DISCONNECTED, nextFlashDisconnected);
  ledFlash(led2, INTERVAL_DISCONNECTED, nextFlashDisconnected);
}

void ledBMP280failed() {
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
}

void startUpFlash() {
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  delay(500);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  delay(500);
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  delay(200);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  delay(200);
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  delay(200);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
}