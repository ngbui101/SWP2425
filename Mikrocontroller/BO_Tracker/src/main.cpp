#include <Arduino.h>
#include <Wire.h>  // Include Wire library for I2C communication
#include "arduino_bma456.h" 
 // BMA456 library (make sure this is correctly included)

#define DSerial SerialUSB
#define ATSerial Serial1

#define INTERRUPT_PIN 22  // Interrupt Pin (PA22) - not used for now

// Variables to store sensor data
float x, y, z;
int32_t temp = 0;
uint32_t step = 0;



void setup() {
    DSerial.begin(115200);
    delay(3000);
    ATSerial.begin(115200);
    delay(3000);

    // Initialize I2C
    Wire.begin();  // Start I2C communication

    // Initialize BMA456 sensor with I2C communication
    bma456.initialize(RANGE_4G, ODR_1600_HZ, NORMAL_AVG4, CONTINUOUS);
    Serial.println("Sensor initialized!");
}

void loop() {
    bma456.getAcceleration(&x, &y, &z);

    temp = bma456.getTemperature();

    Serial.print("X: ");
    Serial.print(x);
    Serial.print(", Y: ");
    Serial.print(y);
    Serial.print(", Z: ");
    Serial.print(z);
    Serial.print(", T: ");
    Serial.println(temp);
    step = bma456.getStepCounterOutput();

    Serial.print("Step: ");
    Serial.println(step);

    delay(5000);
}
