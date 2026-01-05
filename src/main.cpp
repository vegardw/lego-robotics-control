#include <Arduino.h>
#include <MovingBricks.h>


void touchButtonCallback();

void setup() {
  Serial.begin(9600);
  MovingBricks.begin();
  MovingBricks.setTouchButton(touchButtonCallback);
}

void loop() {
  // Simulate main loop doing other tasks
  delay(500);
}

// Example callback function for touch sensor events,
// prints the current state to Serial Monitor.
// This function is called from an ISR context, so it should be kept short.
// Serial printing is generally not recommended in ISRs as they can block,
// but is used here for demonstration purposes. Do not do this in production code.
void touchButtonCallback() {
  MBTouchState state = MovingBricks.getTouchState();

  if(state == MBTouchState::PRESSED) {
    Serial.println("Touch Sensor Pressed");
  } else if(state == MBTouchState::RELEASED) {
    Serial.println("Touch Sensor Released");
  }
}