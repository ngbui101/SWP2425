#include <Arduino.h>

#define BATVOLTAGE_CTRL_PIN A11  // Control pin to enable the voltage divider
#define BATVOLTAGE_READ_PIN A1     // Analog input pin to read the battery voltage
#define REF_VOLTAGE 3.3            // Reference voltage of the MCU (SAMD board is 3.3V)
#define R25 10000                  // Resistor value R25 in ohms (10k)
#define R26 56000                  // Resistor value R26 in ohms (56k)
#define FULL_BATTERY_VOLTAGE 4.2   // Voltage at 100% battery charge
#define EMPTY_BATTERY_VOLTAGE 3.0  // Voltage at 0% battery charge

// Function to initialize pins
void setup()
{
    Serial.begin(115200);         // Initialize Serial for debugging
    pinMode(BATVOLTAGE_CTRL_PIN, OUTPUT);  // Set PA11 as output for controlling transistor
    digitalWrite(BATVOLTAGE_CTRL_PIN, LOW);  // Initially disable the voltage divider (transistor off)
    delay(1000);                  // Wait for things to stabilize
}

// Function to read the battery voltage
float readBatteryVoltage()
{
    digitalWrite(BATVOLTAGE_CTRL_PIN, HIGH);  // Enable voltage measurement (turn transistor Q3 on)
    delay(100);                               // Small delay for stabilization

    int analogValue = analogRead(BATVOLTAGE_READ_PIN);  // Read the analog value
    float measuredVoltage = (analogValue * REF_VOLTAGE) / 1023.0;  // Convert to voltage

    // Battery voltage calculation based on voltage divider
    float batteryVoltage = measuredVoltage * (R25 + R26) / R25;

    digitalWrite(BATVOLTAGE_CTRL_PIN, LOW);   // Disable voltage measurement (turn transistor Q3 off)
    
    return batteryVoltage;
}

// Function to calculate battery percentage based on voltage
float calculateBatteryPercentage(float batteryVoltage)
{
    if (batteryVoltage >= FULL_BATTERY_VOLTAGE)
        return 100.0;  // Cap percentage at 100%
    else if (batteryVoltage <= EMPTY_BATTERY_VOLTAGE)
        return 0.0;    // Cap percentage at 0%
    
    // Linearly interpolate the percentage based on the battery voltage range
    return ((batteryVoltage - EMPTY_BATTERY_VOLTAGE) / (FULL_BATTERY_VOLTAGE - EMPTY_BATTERY_VOLTAGE)) * 100.0;
}

void loop()
{
    float batteryVoltage = readBatteryVoltage();  // Get the battery voltage
    float batteryPercentage = calculateBatteryPercentage(batteryVoltage);  // Calculate remaining battery percentage
    
    // Print the battery voltage and percentage
    Serial.print("Battery Voltage: ");
    Serial.print(batteryVoltage, 2);  // Print voltage with 2 decimal places
    Serial.print(" V, ");
    
    Serial.print("Battery Percentage: ");
    Serial.print(batteryPercentage, 1);  // Print percentage with 1 decimal place
    Serial.println(" %");

    delay(5000);  // Wait for 5 seconds before the next reading
}
