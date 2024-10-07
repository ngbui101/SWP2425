#include "Battery.h"

_Battery::_Battery() 
{
}

_Battery::~_Battery()
{
}

bool _Battery::InitBattery()
{
    pinMode(BATVOLTAGE_CTRL_PIN, OUTPUT);
    digitalWrite(BATVOLTAGE_CTRL_PIN, LOW);
    delay(1000);
    return true;
}

float _Battery::readBatteryVoltage()
{
    digitalWrite(BATVOLTAGE_CTRL_PIN, HIGH);
    delay(100);

    int analogValue = analogRead(BATVOLTAGE_READ_PIN);
    float measuredVoltage = (analogValue * REF_VOLTAGE) / 1023.0;
    float batteryVoltage = measuredVoltage * (R25 + R26) / R25;

    digitalWrite(BATVOLTAGE_CTRL_PIN, LOW);

    return batteryVoltage;
}

float _Battery::calculateBatteryPercentage(float batteryVoltage)
{
    if (batteryVoltage >= FULL_BATTERY_VOLTAGE)
        return 100.0;
    else if (batteryVoltage <= EMPTY_BATTERY_VOLTAGE)
        return 0.0;
    return ((batteryVoltage - EMPTY_BATTERY_VOLTAGE) / (FULL_BATTERY_VOLTAGE - EMPTY_BATTERY_VOLTAGE)) * 100.0;
}
