#ifndef _BATTERY_H_  
#define _BATTERY_H_

#include <Arduino.h>

#define BATVOLTAGE_CTRL_PIN 3u
#define BATVOLTAGE_READ_PIN A1
#define REF_VOLTAGE 3.3          // Reference voltage of the MCU (SAMD board is 3.3V)
#define R25 10000                // Resistor value R25 in ohms (10k)
#define R26 56000                // Resistor value R26 in ohms (56k)
#define FULL_BATTERY_VOLTAGE 4.2 // Voltage at 100% battery charge
#define EMPTY_BATTERY_VOLTAGE 3.0

class _Battery
{
public:
    _Battery();
    ~_Battery();
    bool InitBattery();
    float calculateBatteryPercentage(float batteryVoltage);
    float readBatteryVoltage();

private:
};

#endif 