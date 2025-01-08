#ifndef _LOWPOWER_H_
#define _LOWPOWER_H_

#include "Motion.h"
#include <ArduinoLowPower.h>

class _Lowpower : public _Motion {
public:
    _Lowpower();
    ~_Lowpower();

    void deepSleep(int millis);

private:
};

#endif // _LOWPOWER_H_
