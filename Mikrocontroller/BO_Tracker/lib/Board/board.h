
#ifndef __BOARD_H_
#define __BOARD_H_
#include <RTCZero.h>
#include "Temperature.h"
#include "ArduinoJson.h"

RTCZero rtc;

class _Board : public _Temperature{
    public:
    _Board();
    boolean initBoard();

    bool setupRTC(uint8_t hh, uint8_t mm, uint8_t ss, uint8_t _dd, uint8_t _mm, uint8_t _yyyy);

    bool getDateTime(JsonDocument &docInput);
};
#endif
