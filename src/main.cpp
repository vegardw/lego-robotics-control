#include <Arduino.h>

#define MS_INPUT_PIN_1 A0  // PIN 1: Analog input with external 10k ohm pull-up resistor to 5V

enum SensorState {
  SENSOR_RELEASED = 0,
  SENSOR_PRESSED  = 1
};

int touchSensorStete = SENSOR_RELEASED;

void setup() {
  pinMode(MS_INPUT_PIN_1, INPUT);         // Use external 10k ohm pull-up resistor to 5V
  Serial.begin(9600);
}

void loop() {
  // Simple touch sensor logic using analog read on pin 1
  // No button debouncing for simplicity, need to be added for real applications
  int pin1Voltage = analogRead(MS_INPUT_PIN_1) * (5.0 / 1023.0) * 1000; // mV
  int newTouchSensorState = touchSensorStete;

  if(pin1Voltage > 850 && pin1Voltage < 950) {
    newTouchSensorState = SENSOR_PRESSED;
  } else if (pin1Voltage > 4800) {
    newTouchSensorState = SENSOR_RELEASED;
  }

  if(newTouchSensorState != touchSensorStete) {
    touchSensorStete = newTouchSensorState;
    if(touchSensorStete == SENSOR_PRESSED) {
      Serial.println("Touch Sensor Pressed");
    } else {
      Serial.println("Touch Sensor Released");
    }
  }
  delay(100);
}