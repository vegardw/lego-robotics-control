#include "MovingBricks.h"
#include <Arduino.h>

// ISR for Timer2 compare match. Since Timer2 is only 8-bit,
// we use a counter to achieve ~50 ms intervals (49 * ~1.024 ms ≈ 50.176 ms)
// This polling rate, combined with debounceThreshold, determines stability time.
// With the default debounceThreshold of 2, we get ~100 ms debounce time.
ISR(TIMER2_COMPA_vect) {
    static int isrCount = 0;
    isrCount++;
    if (isrCount >= 49) { 
        if(MovingBricks.getSensorType() == MBSensorType::NXT_TOUCH)
            MovingBricks.readTouchSensor();
        isrCount = 0;
    }
}

void MovingBricks_::begin() {
    // Pin definitions for Mindstorms NXT/EV3 plug.
    pinMode(MS_INPUT_PIN_1, INPUT); // Has external 10k ohm pull-up resistor to 5V
                                    // so don't enable internal pull-up

    // Setup Timer2 for automatic polling of sensors
    cli();                          // Disable interrupts
    TCCR2A = (1 << WGM21);          // CTC mode
    TCCR2B = (1 << CS22);           // Prescaler 64
    OCR2A = 255;                    // Overflow at 256 ticks for ~1.024 ms
    TIMSK2 |= (1 << OCIE2A);        // Enable compare match interrupt
    sei();                          // Enable interrupts
    
    // All setup done, set initialized flag
    initialized = true;
}

void MovingBricks_::end() {
    initialized = false;            // Clear initialized flag
    touchButtonCallback = nullptr;  // Clear callback functions
    cli();                          // Disable interrupts
    TIMSK2 &= ~(1 << OCIE2A);       // Disable compare match interrupt
    sei();                          // Enable interrupts
}

void MovingBricks_::readTouchSensor() {
    // Only process if initialized and sensor is NXT_TOUCH
    if (!initialized || sensorType != MBSensorType::NXT_TOUCH) {
        return;
    }

    MBTouchState newRawState = readRawTouchState();

    // Debouncing logic, using a simple counter
    // To tune the debounce time, adjust debounceThreshold using setDebounceThreshold()
    // Default is 2, meaning ~100ms stability required (2 * ~50ms)
    if (newRawState != touchState) {
        if (newRawState == rawTouchState) {
            debounceCounter++;
            if (debounceCounter >= debounceThreshold) {
                touchState = newRawState;
                debounceCounter = 0;
                if (touchButtonCallback != nullptr) {
                    touchButtonCallback();
                }
            }
        } else {
            rawTouchState = newRawState;
            debounceCounter = 1;
        }
    } else {
        debounceCounter = 0;
    }
}

void MovingBricks_::setTouchButton(MBSensorCallback callback) {
    sensorType = MBSensorType::NXT_TOUCH;
    // Initial read to set baseline state without triggering callback
    touchState = readRawTouchState();
    touchButtonCallback = callback;
}

MBTouchState MovingBricks_::readRawTouchState() {
    cli(); // Disable interrupts to ensure consistent ADC reading
    int pin1Voltage = analogRead(MS_INPUT_PIN_1) * (5.0 / 1023.0) * 1000; // mV
    sei(); // Re-enable interrupts

    MBTouchState state;
    if (pin1Voltage > 850 && pin1Voltage < 950) {
        state = MBTouchState::PRESSED;
    } else if (pin1Voltage > 4800) {
        state = MBTouchState::RELEASED;
    } else {
        state = MBTouchState::NONE;
    }

    // Basic fault detection: If state is NONE too many times in a row, mark as fault
    // This assumes persistent invalid readings indicate a wiring/sensor issue
    // Reset fault if we get a valid reading
    if (state == MBTouchState::NONE) {
        faultCounter++;
        if (faultCounter >= faultThreshold) {
            sensorFault = true;
        }
    } else {
        faultCounter = 0;
        sensorFault = false;  // Clear fault on valid reading
    }

    return state;
}

MovingBricks_ &MovingBricks_::getInstance() {
  static MovingBricks_ instance;
  return instance;
}

MovingBricks_ &MovingBricks = MovingBricks_::getInstance();