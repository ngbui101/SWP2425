#include "Lowpower.h"

_Lowpower::_Lowpower() : _Motion() {
}

_Lowpower::~_Lowpower() {
}

void _Lowpower::deepSleep(int millis)
{
    if (millis <= 10000)
    {
        LowPower.deepSleep(10000);
    }
    LowPower.deepSleep(millis);
}