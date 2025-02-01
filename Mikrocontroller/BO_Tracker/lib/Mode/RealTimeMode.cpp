#include "RealTimeMode.h"

RealTimeMode::RealTimeMode(Tracker &trackerObj)
    : tracker(trackerObj)
{
}

void RealTimeMode::start()
{   
    // Serial.println("Real Time Start");
    // tracker.blink();
    if (!setup())
        return;
    loop();
}

bool RealTimeMode::setup()
{   
    bool useMQTT = false;
    return tracker.turnOnFunctionality(useMQTT);
}

// Hauptschleife (z. B. zyklische Abfragen, Publikationen etc.)
void RealTimeMode::loop()
{   
    bool keepRunning = true;
    while (keepRunning)
    {   
        // Serial.println("keepRunning");
        // keepRunning = tracker.pubAndsubMQTT(trackerModes.period);
        keepRunning = tracker.sendAndWaitResponseHTTP(trackerModes.period);
    }
    // Serial.println("Running handleErrors()");
    tracker.handleErrors();
    // tracker.handleErrors();
}
