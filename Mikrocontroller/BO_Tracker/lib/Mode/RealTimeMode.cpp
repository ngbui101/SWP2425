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
    if (trackerModes.GnssMode && !tracker.isGnssModuleEnable())
    {
        tracker.TurnOnGNSS();
    }
}

// Hauptschleife (z. B. zyklische Abfragen, Publikationen etc.)
void RealTimeMode::loop()
{
    unsigned long pub_time = millis() + trackerModes.period;

    bool keepRunning = true;
    int count = 0;
    while (keepRunning)
    {
        unsigned long start = millis();
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
        if (abs(millis() - pub_time) >= trackerModes.period - 1000)
        {
            tracker.modeHandle();
            if (tracker.publishData("/pub"))
            {
                pub_time = millis();
            }
            else
            {
                keepRunning = false;
            }
            // char infos[50];
            // tracker.getModem().GetDevInformation(infos);
            pub_time = millis();
            // Serial.println("Public Data");
        }

        delay(100);
    }
}
