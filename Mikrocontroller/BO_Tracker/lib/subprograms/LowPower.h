#ifndef _LOWPOWER_H_
#define _LOWPOWER_H_

#include "Motion.h"
#include <ArduinoLowPower.h>

class _Lowpower : public _Motion {
public:
    _Lowpower();
    ~_Lowpower();

    bool deepSleep(unsigned long millis);

    bool deepSleepWithAlarm(unsigned long millis);
private:
    int countMaxSleep = 0;
    int countMinSleep = 0;
    uint32_t maxSleepTime = 120000;
    uint32_t minSleepTime = 10000;
};

#endif // _LOWPOWER_H_
