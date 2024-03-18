const unsigned long BUTTON_HOLD_TIME = 3000; // The number of millis for which the button must be holded
const unsigned long DEBOUNCE_DELAY = 50;    // the debounce time; increase if the output flickers
const unsigned long BUTTON_UPDATE_DELAY_AFTER_MEASUREMENT = 50; // time to wait for a button state update after a gas discharge or vessel length measurement

void buttonCheckChangeMode() { // check if button 1 is pressed for 3 seconds to change mode
  static bool changeMode = false; // Stores if it is enable to change mode
  static unsigned long btn1HoldStart = BUTTON_HOLD_TIME; // The millis() value at which holding button 1 started // colocar 15min?
  static ButtonState btn1PreviousState; // The previous state of btn1
  
  ButtonState btn1CurrentState = buttonRead(btn1); // The current state of btn1

  if ((millis() - lastMeasurementTime) < BUTTON_UPDATE_DELAY_AFTER_MEASUREMENT) {
    // Check if this makes the bug of changing mode after measurement stops
    changeMode = false;
    btn1PreviousState = UP;
    btn1HoldStart = millis();
    Serial.println("Waiting before read button1 state just after measurement...");
    return;
  }

  if (btn1PreviousState == UP && btn1CurrentState == DOWN) { // Up to Down - "Press"
    changeMode = true; // It's true the first time it's pressed
    btn1HoldStart = millis(); // The button was just pressed, store start time of holding
    btn1PreviousState = btn1CurrentState;
    return;
  }
 
  if (btn1PreviousState == DOWN && btn1CurrentState == UP) { // Down to Up - "Release"
    changeMode = false; // It's needed to press the button again to change mode
    btn1PreviousState = btn1CurrentState;
    return;
  }

  if (btn1PreviousState == DOWN && btn1CurrentState == DOWN && millis() > btn1HoldStart + BUTTON_HOLD_TIME) { // Hold Down
    if (changeMode) {
      changeMode = false; // It's needed to realese and press the button again to change mode
      if (deviceMode == GAS_DISCHARGE) {
        deviceMode = VESSEL_LENGTH;
      } else {
        deviceMode = GAS_DISCHARGE;
      }
    }
    return;
  }
}

bool buttonCheckNewMeasure() { // check if button 2 is pressed using a debounce timer
  static ButtonState buttonState; // button state with debounce correction
  static ButtonState lastButtonState = UP;  // the previous reading from the input pin
  static unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
  
  ButtonState currentState = buttonRead(btn2);

  // If the switch changed, due to noise or pressing:
  if (currentState != lastButtonState) {
    lastDebounceTime = millis(); // reset the debouncing timer
  }
  lastButtonState = currentState; // update lastButtonState

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (currentState != buttonState) {
      buttonState = currentState;

      // only trigger a new measurement if button2 was pressed
      if (buttonState == DOWN) {
        return true;
      }
    }
  }
  return false;
}

ButtonState buttonRead(int button) {
  return digitalRead(button) == HIGH ? UP : DOWN;
}
