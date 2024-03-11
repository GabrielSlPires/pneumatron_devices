const int INTERVAL_FLASH_AIR_DISCHARGE = 1000; // This is our Flash Interval for AIR_DISCHARGE (AD) mode
unsigned long nextFlashAD; // This will hold the millis() value of our next Flash

void FlashAD() {
  if (millis() < nextFlashAD) {
    return; // It's not time yet
  }
  digitalWrite(led1, !digitalRead(led1)); // Toggle the LED State
  nextFlashAD = millis() + INTERVAL_FLASH_AIR_DISCHARGE; // Sets the time it should toggle again.
}

// This will flash led2 every 100ms with meassage delivery failed
const int INTERVAL_FLASH_DISCONNECTED = 100;
unsigned long nextFlashDisconnected;

void FlashDisconnected() {
  if (millis() < nextFlashDisconnected) {
    return;
  }
  digitalWrite(led2, !digitalRead(led2)); // Toggle the LED State
  nextFlashDisconnected = millis() + INTERVAL_FLASH_DISCONNECTED; // Sets the time it should toggle again.
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