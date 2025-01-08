#include "RealTimeMode.h"

RealTimeMode::RealTimeMode(Tracker &trackerObj)
    : tracker(trackerObj)
{
}

void RealTimeMode::start()
{
    if (!setup())
        return;
    loop();
}

bool RealTimeMode::setup()
{
    return tracker.turnOnFunctionality();
}

// Hauptschleife (z. B. zyklische Abfragen, Publikationen etc.)
void RealTimeMode::loop()
{
    bool keepRunning = true;
    while (keepRunning)
    {
        keepRunning = tracker.sendAndCheck();
    }
}
