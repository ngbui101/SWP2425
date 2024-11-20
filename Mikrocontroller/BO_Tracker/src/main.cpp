#include <Arduino.h>
#include <Wire.h>  // Include Wire library for I2C communication
#include "arduino_bma456.h"  // BMA456 library (make sure this is correctly included)

#define DSerial SerialUSB
#define ATSerial Serial1

#define INTERRUPT_PIN 22  // Interrupt Pin (PA22) - not used for now

// Variables to store sensor data
float previousX = 0, previousY = 0, previousZ = 0;
float x, y, z;

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
}

void loop() {
    // Read current sensor data (acceleration)
    bma456.getAcceleration(&x, &y, &z);
    
    // Print sensor values to the Serial Monitor
    Serial.print("Acceleration [X, Y, Z]: ");
    Serial.print(x); Serial.print(", ");
    Serial.print(y); Serial.print(", ");
    Serial.println(z);
    
    // Check if there is significant motion (change in any axis)
    if (abs(x - previousX) > 0.1 || abs(y - previousY) > 0.1 || abs(z - previousZ) > 0.1) {
        Serial.println("Motion detected!");
    }

    // Save current sensor values for comparison in the next loop
    previousX = x;
    previousY = y;
    previousZ = z;

    delay(500);  // Delay for a while to avoid flooding the Serial Monitor
}
