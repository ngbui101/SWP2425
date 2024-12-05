
#ifndef __BOARD_H_
#define __BOARD_H_
#include <RTCZero.h>
#include "Temperature.h"
#include "ArduinoJson.h"

class _Board : public _Temperature
{
public:
    _Board();
    bool initBoard();

    bool setupRTCFromModem(const char *modemTime);

    char *getDateTime();

private:
    RTCZero rtc;
    
};

#endif
