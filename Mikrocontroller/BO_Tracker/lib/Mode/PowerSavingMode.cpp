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
        if (!tracker.turnOnFunctionality())
        {
            return;
        }
        if (!tracker.sendAndWaitResponseHTTP())
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
            tracker.turnOffModem();
        }
        tracker.deepSleepWithAlarm(trackerModes.period);
    }
}
