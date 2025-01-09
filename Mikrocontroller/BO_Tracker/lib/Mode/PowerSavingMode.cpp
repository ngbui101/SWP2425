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
        if (!tracker.turnOnFunctionality())
            return;
        // delay(5000);
        // Serial.println("SendData");
        // tracker.blink();
        if (!tracker.sendAndWaitResponseHTTP())
        {
            tracker.handleErrors();
            return;
        }
        trackerModes.wakeUp = false;
    }
    else
    {
        if (tracker.isModemAvailable())
        {
            // Serial.println("TurnOffModem");
            tracker.turnOffModem();
        }
        // Serial.println("GoTo Sleep");
        tracker.deepSleepWithAlarm(trackerModes.period - 10 * 1000ul);
    }
}

// bool PowerSavingMode::setup()
// {
//     return tracker.turnOnFunctionality();
// }

// Hauptschleife (z. B. zyklische Abfragen, Publikationen etc.)
// bool PowerSavingMode::sendData()
// {
//     if (!tracker.sendAndWaitResponseHTTP())
//     {

//         return false;
//     }
//     return true;
// }
