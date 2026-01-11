#include <Arduino.h>
#include <MovingBricks.h>


void touchButtonCallback();

bool ledState = false;
bool useCallback = false; // Set to true to use callback, false to check if state changed in loop

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
  Serial.begin(9600);
  MovingBricks.begin();
  if (useCallback)
    MovingBricks.setTouchButton(touchButtonCallback);
  else
    MovingBricks.setTouchButton();
}

void loop() {
  if(!useCallback) {
    if(MovingBricks.isTouchStateChanged()) {
      MBTouchState state = MovingBricks.getTouchState(); // This clears the changed flag
      if(state == MBTouchState::PRESSED) {
        ledState = !ledState;
        digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
        Serial.println("Touch Sensor Pressed");
      } else if(state == MBTouchState::RELEASED) {
        Serial.println("Touch Sensor Released");
      }
    }
  }
  delay(50);
}

// Example callback function for touch sensor events,
// prints the current state to Serial Monitor.
// This function is called from an ISR context, so it should be kept short.
// Serial printing is generally not recommended in ISRs as they can block,
// but is used here for demonstration purposes. Do not do this in production code.
// If you need to perform lengthy operations, set the callback to nullptr
// and check for state changes using isTouchStateChanged() in the main loop instead,
// or have the callback set a flag that is processed in the main loop.
void touchButtonCallback() {
 
  MBTouchState state = MovingBricks.getTouchState();

  if(state == MBTouchState::PRESSED) {
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
    Serial.println("Touch Sensor Pressed");
  } else if(state == MBTouchState::RELEASED) {
    Serial.println("Touch Sensor Released");
  }
}