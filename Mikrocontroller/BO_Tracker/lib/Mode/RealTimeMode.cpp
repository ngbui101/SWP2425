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
        Serial.println("turnOnModem");
        if (!tracker.turnOnModem())
            return;
    }
    if (trackerModes.GnssMode && !tracker.isGnssModuleEnable())
    {
        Serial.println("TurnOnGNSS");
        tracker.TurnOnGNSS();
    }
    if (!tracker.isConnected())
    {
        Serial.println("startConnect");
        tracker.startConnect();
    }
    if (!tracker.isUrlSetted())
    {
        Serial.println("setHTTPURL");
        tracker.setHTTPURL(http_url);
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
