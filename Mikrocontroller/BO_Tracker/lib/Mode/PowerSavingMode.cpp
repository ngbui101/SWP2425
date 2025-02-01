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
        bool useMQTT = false;
        if (!tracker.turnOnFunctionality(useMQTT))
        {
            return;
        }
        if (!tracker.sendAndWaitResponseHTTP(trackerModes.maxRealTime))
        {
            tracker.handleErrors();
            return;
        }
        trackerModes.wakeUp = false;

        // Serial.print("\nZaeler: ");
        // Serial.println(zaehler++);
    }
    else
    {
        if (tracker.isModemAvailable())
        {
            // Serial.println("TurnOffModem");
            tracker.turnOffModem();
        }
        tracker.deepSleepWithAlarm(trackerModes.period);
    }
}
