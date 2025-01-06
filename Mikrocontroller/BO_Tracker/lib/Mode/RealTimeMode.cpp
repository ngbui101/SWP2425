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
    if (!tracker.isMQTTAvaliable())
    {
        tracker.startMQTT();
    }
    if (!tracker.isGnssModuleEnable())
    {
        tracker.TurnOnGNSS();
    }
}

// Hauptschleife (z. B. zyklische Abfragen, Publikationen etc.)
void RealTimeMode::loop()
{
    unsigned long pub_time = 0;
    bool keepRunning = true;
    while (keepRunning)
    {
        unsigned long start_time = millis();
        char response[1028];
        Mqtt_Event_t event = tracker.waitForResponse(response);
        switch (event)
        {
        case MQTT_RECV_DATA_EVENT:
            tracker.setMode(response);
            keepRunning = false;
            break;
        case MQTT_CLIENT_CLOSED:
            keepRunning = false;
            break;
        default:
            break;
        }
        if ((start_time - pub_time) >= trackerModes.period)
        {
            tracker.modeHandle();
            if (tracker.publishData("/pub"))
            {
                pub_time = millis();
            }
        }
        delay(1000);
    }
}
