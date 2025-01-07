#include "RealTimeMode.h"

RealTimeMode::RealTimeMode(Tracker &trackerObj)
    : tracker(trackerObj)
{
}

void RealTimeMode::start()
{
    setup();
    loop();
}

void RealTimeMode::setup()
{   
    if (!tracker.isModemAvailable())
    {
        tracker.turnOnModem();
    }
    if (trackerModes.GnssMode && !tracker.isGnssModuleEnable())
    {
        tracker.TurnOnGNSS();
    }
    // if (!tracker.isMQTTAvaliable())
    // {
    //     tracker.startMQTT();
    // }
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
