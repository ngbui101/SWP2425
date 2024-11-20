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
  delay(2000);
  ATSerial.begin(115200);
delay(2000);
    // Initialize sensor and I2C
    Wire.begin();
    bma456.initialize(RANGE_4G, ODR_50_HZ, NORMAL_AVG4, CIC_AVG);
    Serial.println("Sensor initialized!");
    struct bma4_dev* accel = bma456.getAccelInstance();

    // Set wake-up sensitivity to most sensitive 0-7: most to least sensitive
    uint8_t sensitivity = 0x00;
    bma456_wakeup_set_sensitivity(sensitivity, accel);

    // Configure any-motion detection (set threshold and duration)
    struct bma456_anymotion_config motion_config;
    motion_config.threshold = 0xAA; // 83mg threshold
    motion_config.duration = 5; // 100ms
    motion_config.nomotion_sel = 0; // Any-motion
    bma456_set_any_motion_config(&motion_config, accel);

    // Map the motion interrupt to pin 22
    bma456_map_interrupt(BMA4_INTR1_MAP, BMA456_ANY_MOTION, BMA4_ENABLE, accel);
    pinMode(INTERRUPT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), motionInterruptHandler, RISING);
}

void loop() {
    // Read and display motion data
    
    if (motionDetected) {
        motionDetected = false; // Reset motion detection flag
        Serial.println("Motion detected!");
        readSensorData();
    }
}

void motionInterruptHandler() {
    motionDetected = true;  // Set flag for motion detection
    Serial.println("Interrupt triggered");
}

void readSensorData() {
    // Read acceleration values
    float x, y, z;
    bma456.getAcceleration(&x, &y, &z);
    Serial.print("Acceleration [X, Y, Z]: ");
    Serial.print(x); Serial.print(", ");
    Serial.print(y); Serial.print(", ");
    Serial.println(z);
}

