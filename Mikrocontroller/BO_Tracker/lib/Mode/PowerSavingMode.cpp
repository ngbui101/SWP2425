#include "PowerSavingMode.h"

PowerSavingMode::PowerSavingMode(Tracker &trackerObj)
    : tracker(trackerObj)
{
}

void PowerSavingMode::start()
{
    if (!setup())
        return;
    loop();
}

bool PowerSavingMode::setup()
{
    tracker.turnOnFunctionality();
}

// Hauptschleife (z. B. zyklische Abfragen, Publikationen etc.)
void PowerSavingMode::loop()
{
    Serial.println("PowerSavingMode");
    delay(1000);
}
