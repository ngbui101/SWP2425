
#ifndef __BOARD_H_
#define __BOARD_H_
#include <RTCZero.h>
#include "Temperature.h"
#include "ArduinoJson.h"

const int wakeUpPin = 0;

class _Board : public _Temperature
{
public:
    bool wake_up = false;
    _Board();
    bool initBoard();

    bool setupRTCFromModem(const char *modemTime);

    char *getDateTime();

    bool waitWakeOnMotions();

    void deepSleep();

    bool stillOnMotions();
private:
    RTCZero rtc;
};

#endif
