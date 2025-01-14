#include "Lowpower.h"

_Lowpower::_Lowpower() : _Motion()
{
}

_Lowpower::~_Lowpower()
{
}

bool _Lowpower::deepSleep(unsigned long millis)
{
    millis = (millis <= minSleepTime) ? minSleepTime : maxSleepTime;

    LowPower.deepSleep(millis);
    // Serial.println("Simulate Deep Sleep");
    // delay(millis / 10);
    return true;
}

bool _Lowpower::deepSleepWithAlarm(unsigned long millis)
{
    if (countMinSleep != 0)
    {   
        if ((countMinSleep == 1) && (countMaxSleep == 0))
        {
        countMinSleep--;
        trackerModes.wakeUp = true;
        return true;
        }
        countMinSleep--;
        deepSleep(minSleepTime);
        return true;
    }else if (countMaxSleep != 0)
    {
        countMaxSleep--;
        deepSleep(maxSleepTime);
        return true;
        // first start nicht schlafen aber wake up
    }
    else if ((countMaxSleep == 0) && (countMinSleep == 0))
    {
        countMaxSleep = millis / maxSleepTime;
        countMinSleep = (millis % maxSleepTime) / minSleepTime;
        // Serial.print("Go to Sleep for: ");
        // Serial.print(countMaxSleep);
        // Serial.print("x 2 Minutes and ");
        // Serial.print(countMinSleep);
        // Serial.println("x 10 Seconds");
        trackerModes.wakeUp = true;

        return true;
    }
    return true;
}