
#ifndef __BOARD_H_
#define __BOARD_H_
#include <RTCZero.h>
#include "Temperature.h"
#include "ArduinoJson.h"

#define WAKE_UP_INT_PIN 0u;
const int wakeUpPin = 0;
bool motion = false;

void onMotion();

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
