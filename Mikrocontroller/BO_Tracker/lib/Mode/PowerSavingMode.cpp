#include "PowerSavingMode.h"

PowerSavingMode::PowerSavingMode(Tracker &trackerObj)
    : tracker(trackerObj)
{
}

void PowerSavingMode::start()
{
    if (tracker.wakeUp())
    {
        Serial.println("Wake Up");
        trackerModes.wakeUp = false;
    }
    else
    {
        Serial.println("GoTo Sleep");
        if (tracker.isModemAvailable())
        {
            tracker.turnOffModem();
        }
        delay(120000);
    }
    if (!setup())
        return;
    loop();
}

bool PowerSavingMode::setup()
{
    tracker.enableAlarm(trackerModes.period);
    return tracker.turnOnFunctionality();
}

// Hauptschleife (z. B. zyklische Abfragen, Publikationen etc.)
void PowerSavingMode::loop()
{
    bool keepRunning = true;
    while (keepRunning)
    {
        keepRunning = tracker.sendAndCheck();
    }
    Serial.println("GoTo Sleep");
    if (tracker.isModemAvailable())
    {
        tracker.turnOffModem();
    }
}
