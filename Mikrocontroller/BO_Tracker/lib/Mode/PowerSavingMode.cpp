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
    return tracker.turnOnFunctionality();
}

// Hauptschleife (z. B. zyklische Abfragen, Publikationen etc.)
void PowerSavingMode::loop()
{   
    bool keepRunning = true;
    while (keepRunning)
    {
        
    }
    
}
