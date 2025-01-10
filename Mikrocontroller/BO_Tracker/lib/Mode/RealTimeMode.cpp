#include "RealTimeMode.h"

RealTimeMode::RealTimeMode(Tracker &trackerObj)
    : tracker(trackerObj)
{
}

void RealTimeMode::start()
{   
    Serial.println("Real Time Start");
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
        // Serial.println("keepRunning");
        keepRunning = tracker.sendAndCheck();
    }
    // Serial.println("Running handleErrors()");
    tracker.handleErrors();
    // tracker.handleErrors();
}
