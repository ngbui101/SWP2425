#include <Arduino.h>
#include <Wire.h>  // Include Wire library for I2C communication
#include "arduino_bma456.h"  // BMA456 library (make sure this is correctly included)

#define DSerial SerialUSB
#define ATSerial Serial1

#define INTERRUPT_PIN 22  // Interrupt Pin (PA22)

volatile bool motionDetected = false;  // Flag to indicate motion detection

// Function prototypes
void setupMotionInterrupt();
void motionInterruptHandler();
//void enterSleepMode();
void readSensorData();

void setup() {
    DSerial.begin(115200);
    delay(3000);
    ATSerial.begin(115200);
    delay(3000);

    // Initialize I2C
    Wire.begin();  // Start I2C communication

    // Initialize BMA456 sensor with I2C communication
    bma456.initialize(RANGE_4G, ODR_50_HZ, NORMAL_AVG4, CIC_AVG);
    Serial.println("Sensor initialized!");
    

    // Setup motion interrupt for wake-up
    setupMotionInterrupt();
}

void loop() {
    Serial.println("Checking for motion...");
    if (motionDetected) {
        motionDetected = false; // Reset the motion detection flag
        Serial.println("Motion detected! Device woke up.");

        // Read and display sensor data
        //readSensorData();

        // Re-enter sleep mode after processing
        //enterSleepMode();
    } else {
        // If no motion detected, enter sleep mode
        //enterSleepMode();
    }
}

void setupMotionInterrupt() {
    /*struct bma4_dev* accel = bma456.getAccelInstance();

    // Modify I2C address to use the primary address (0x18)
    accel->dev_addr = BMA4_I2C_ADDR_PRIMARY;  // Set to primary address (0x18)

    // Map motion interrupt to the appropriate interrupt line
    uint16_t result = bma4_map_interrupt(
        BMA4_INTR1_MAP,        // Interrupt line
        BMA456_ANY_MOTION,     // Map any motion interrupt
        BMA4_ENABLE,           // Enable interrupt
        accel                  // Access the accel via getter
    );

    if (result != 0) {
        Serial.println("Failed to map motion interrupt.");
    } else {
        Serial.println("Motion interrupt mapped successfully.");
    }

    // Configure the interrupt pin (Pin 22 on Arduino Zero)
    pinMode(INTERRUPT_PIN, INPUT_PULLUP);  // Enable pull-up resistor on interrupt pin
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), motionInterruptHandler, RISING);  // Trigger on rising edge*/
        Serial.println("Motion interrupt setup skipped for debugging.");
}

/*
void enterSleepMode() {
    // Put the Arduino Zero into sleep mode to save power
    Serial.println("Entering sleep mode...");
    delay(100);  // Ensure all pending prints are flushed

    // Disable the watchdog timer before entering sleep (if necessary)
    WDT->CTRL.bit.ENABLE = 0;

    // Enable external interrupt on pin 22 (used for motion detection)
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), motionInterruptHandler, RISING);

    // Enter deep sleep (wait for interrupt)
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    __WFI(); // Wait For Interrupt - this puts the MCU to sleep until an interrupt occurs

    // Once motion interrupt occurs, the program will wake up here
    Serial.println("Woke up from sleep mode!");
}*/

void readSensorData() {
    // Get acceleration data from the sensor
    float x, y, z;
    bma456.getAcceleration(&x, &y, &z);
    Serial.print("Acceleration [X, Y, Z]: ");
    Serial.print(x); Serial.print(", ");
    Serial.print(y); Serial.print(", ");
    Serial.println(z);

    // Get the temperature data from the sensor
    int32_t temperature = bma456.getTemperature();
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
}

void motionInterruptHandler() {
    motionDetected = true;  // Set flag for motion detection
        Serial.println("Interrupt triggered");

}
