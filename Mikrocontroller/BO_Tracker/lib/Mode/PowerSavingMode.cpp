#include "PowerSavingMode.h"

PowerSavingMode::PowerSavingMode(Tracker &trackerObj)
    : tracker(trackerObj)
{
}

void PowerSavingMode::start()
{
    if (tracker.wakeUp())
    {
        // Serial.println("Wake Up");
        // Serial.println("Setup");
        if (!setup())
            return;
        // Serial.println("SendData");
        sendData();
        trackerModes.wakeUp = false;
    }
    else
    {
        if (tracker.isModemAvailable())
        {   
            // Serial.println("TurnOffModem");
            tracker.enableAlarm(trackerModes.period-10*1000ul); 
            tracker.turnOffModem();
        }
        // Serial.println("GoTo Sleep");
        tracker.deepSleep(120000);
    }
}

bool PowerSavingMode::setup()
{
    tracker.enableAlarm(trackerModes.period);
    return tracker.turnOnFunctionality();
}

// Hauptschleife (z. B. zyklische Abfragen, Publikationen etc.)
void PowerSavingMode::sendData()
{
    while(!tracker.sendAndWaitResponseHTTP()){
        tracker.handleErrors();
    };
}
