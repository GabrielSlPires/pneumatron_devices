const int BUTTON_HOLD_TIME = 3000; // The number of millis for which we must hold the button
unsigned long btn1_hold_start; // The millis() value at which holding button 1 started

bool changeMode = true; // Saves if the mode was changed when pressing the button

ButtonState buttonRead(int button) {
  return digitalRead(button) == HIGH ? UP : DOWN;
}

ButtonState previous_btn1_state;

// check if button 1 is pressed for 3 seconds to change mode
void check_button1_to_change_mode() {
  ButtonState current_btn1_state = buttonRead(btn1);

  // Up to Down - "Press"
  if (previous_btn1_state == UP && current_btn1_state == DOWN) {

    changeMode = true;
    btn1_hold_start = millis(); // The button was just pressed, store start of holding
    previous_btn1_state = current_btn1_state;
    return;
  }

  // Down to Up - "Release"
  if (previous_btn1_state == DOWN && current_btn1_state == UP) {
    previous_btn1_state = current_btn1_state;
    return;
  }

  // Held Down
  if (previous_btn1_state == DOWN && current_btn1_state == DOWN && millis() > btn1_hold_start + BUTTON_HOLD_TIME) {
    if (changeMode) {
      changeMode = false;
      if (deviceMode == AIR_DISCHARGE) {
        deviceMode = VESSEL_LENGTH;
      } else {
        deviceMode = AIR_DISCHARGE;
      }
    }
    return;
  }
}