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
    if (countMaxSleep != 0)
    {
        Serial.print("Max Sleep Count: ");
        Serial.println(countMaxSleep);
        countMaxSleep--;
        deepSleep(maxSleepTime);
        return true;
        // first start nicht schlafen aber wake up
    }
    else if (countMinSleep != 0)
    {
        Serial.print("Min Sleep Count: ");
        Serial.println(countMinSleep);
        if ((countMinSleep == 1) && (countMaxSleep == 0))
        {
            countMinSleep--;
            trackerModes.wakeUp = true;
            return true;
        }
        countMinSleep = 1;
        deepSleep(minSleepTime * (countMinSleep - 1));
        return true;
    }
    else if ((countMaxSleep == 0) && (countMinSleep == 0))
    {
        Serial.println("Set min and max sleep");
        countMaxSleep = millis / maxSleepTime;
        countMinSleep = (millis % maxSleepTime) / minSleepTime;
        if (countMinSleep == 0)
        {
            countMinSleep = 1;
        }
        return true;
    }
    return true;
}