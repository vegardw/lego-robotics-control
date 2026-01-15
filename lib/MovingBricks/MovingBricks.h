/*
 * MovingBricks.h
 * Header file for the MovingBricks library, providing an interface for controlling 
 * different LEGO sensors and actuators with Arduino microcontrollers.
 */

#pragma once

// Pin definitions
#define MS_INPUT_PIN_1 A0  // PIN 1: Analog input with external 10k ohm pull-up resistor to 5V
#define MS_INPUT_PIN_5 A4  // PIN 5: Connected to A4, only used as digital output for now
#define MS_INPUT_PIN_6 A5  // PIN 5: Connected to A4, only used as digital output for now

/// @brief Callback function type for sensor events.
typedef void (*MBSensorCallback)(void);

/// @brief Enumeration for different types of sensors supported by the library.
enum class MBSensorType
{
    NONE,      ///< No sensor configured.
    NXT_TOUCH, ///< LEGO NXT Touch Sensor.
    NXT_LIGHT, ///< LEGO NXT Light Sensor
};

/// @brief Enumeration for the state of a touch sensor.
enum class MBTouchState
{
    NONE,     ///< No state or unknown.
    PRESSED,  ///< Sensor is pressed.
    RELEASED  ///< Sensor is released.
};

/**
 * @class MovingBricks_
 * @brief Singleton class for managing different LEGO sensors and actuators.
 * 
 * This class provides methods to initialize, configure, and read from various LEGO sensors,
 * such as the NXT Touch Sensor. It uses a singleton pattern to ensure only one instance exists.
 */
class MovingBricks_
{
    private:
        MovingBricks_() = default;  ///< Private constructor for singleton pattern.
        MovingBricks_(const MovingBricks_&) = delete;  ///< Deleted copy constructor to enforce singleton.
        MovingBricks_& operator=(const MovingBricks_&) = delete;  ///< Deleted assignment operator to enforce singleton.

        bool initialized = false;  ///< Flag indicating if the library is initialized.

        MBSensorType sensorType = MBSensorType::NONE;  ///< Current sensor type configured.
        
        MBTouchState touchState = MBTouchState::NONE;  ///< Current stable state of the touch sensor.
        bool touchStateChanged = false;  ///< Flag indicating if the touch state has changed since last read.
        volatile MBTouchState rawTouchState = MBTouchState::NONE;  ///< Raw state from ADC readings.
        volatile int debounceCounter = 0;  ///< Counter for debouncing stability.
        int debounceThreshold = 2;  ///< Number of consecutive stable readings required for state change.
        MBSensorCallback touchButtonCallback = nullptr;  ///< Callback function for touch sensor events.
        MBTouchState readRawTouchState();  ///< Read the raw state of the touch sensor.

        bool lightSensorLEDLit = false; ///< Flag indicating if the light sensor LED is lit
        int lightSensorMinAnalog = 62;  ///< Minimum analog value for 100% light
        int lightSensorMaxAnalog = 1023; ///< Maximum analog value for 0% light

        bool sensorFault = false;  ///< Flag indicating if the sensor appears faulty (e.g., invalid voltage range).
        int faultCounter = 0;      ///< Counter for consecutive invalid readings to detect faults.
        int faultThreshold = 10;  ///< Consecutive invalid readings before marking as fault.

        
        
        
        
        
    public:
        /**
         * @brief Initializes the MovingBricks library.
         * Sets the initialized flag to true and enables automatic polling
         * of sensors via Timer2 timer interrupts.
         */
        void begin();
        
        /**
         * @brief Deinitializes the MovingBricks library.
         * Sets the initialized flag to false, clears callback functions, and disables automatic polling.
         */
        void end();

        /**
         * @brief Reads the current state of the touch sensor with debouncing.
         * Updates the touchState and calls the callback if the state changes after debouncing.
         */
        void readTouchSensor();

        /**
         * @brief Configures the sensor as a light sensor.
         * Configures the sensor type to NXT_LIGHT and performs an initial read.
         * @param enableLED Enable built-in LED (default = false).
         */
        void setLightSensor(bool enableLED = false);

                /**
         * @brief Configures the sensor as a touch button.
         * Configures the sensor type to NXT_TOUCH and performs an initial read.
         * @param callback Pointer to the callback function to be called on state change (default = nullptr).
         */
        void setTouchButton(MBSensorCallback callback = nullptr);

        /**
         * @brief Gets the current sensor type.
         * @return The configured sensor type.
         */
        MBSensorType getSensorType() const { return sensorType; }
        
        /**
         * @brief Gets the current touch state.
         * Sets touchStateChanged to false when called.
         * @return The current state of the touch sensor.
         */
        MBTouchState getTouchState();

         /**
         * @brief Returns if the touch state has changed.
         * @return True if the touch state has changed, false otherwise.
         */
        bool isTouchStateChanged() const { return touchStateChanged; }

        /**
         * @brief Gets the current debounce threshold.
         * @return The debounce threshold value.
         */
        int getDebounceThreshold() const { return debounceThreshold; }

        /**
         * @brief Sets the debounce threshold.
         * @param threshold Number of consecutive stable readings (e.g., 2 for ~100ms debounce).
         */
        void setDebounceThreshold(int threshold) { debounceThreshold = threshold; }


        /**
         * @brief Checks if the sensor appears functioning.
         * Returns false if voltage readings are consistently invalid (e.g., wiring issue).
         * This is a basic check; more diagnostics may be added in future versions.
         * @return True if sensor seems connected, false otherwise.
         */
        bool isSensorFunctioning() const { return !sensorFault; }


         /**
         * @brief Gets the fault threshold.
         * @return The number of consecutive invalid readings before marking as fault.
         */
        int getFaultThreshold() const { return faultThreshold; }

        /**
         * @brief Sets the fault threshold.
         * @param threshold Number of consecutive invalid readings (e.g., 10 for default).
         */
        void setFaultThreshold(int threshold) { faultThreshold = threshold; }


        /**
         * @brief Gets the touch button callback function.
         * @return Pointer to the callback function.
         */
        MBSensorCallback getTouchButtonCallback() const { return touchButtonCallback; }

        void enableLightSensorLED(bool enable=true);
        bool getLightSensorLEDLit() const {return lightSensorLEDLit; }
        int getLightSensorValue();

        /**
         * @brief Gets the singleton instance of the MovingBricks class.
         * @return Reference to the singleton instance.
         */
        static MovingBricks_& getInstance();

};

/// @brief Global reference to the singleton instance of MovingBricks_.
extern MovingBricks_ &MovingBricks;