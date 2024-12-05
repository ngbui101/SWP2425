#include "Board.h"

_Board::_Board()
{
}
boolean _Board::initBoard()
{
    if (initBattery() && initTemp())
        return true;
    else
        return false;
}

boolean _Board::setupRTC(uint8_t hh, uint8_t mm, uint8_t ss, uint8_t _dd, uint8_t _mm, uint8_t _yyyy)
{
    rtc.setHours(hh);
    rtc.setMinutes(mm);
    rtc.setSeconds(ss);

    rtc.setDay(_dd);
    rtc.setMonth(_mm);
    rtc.setYear(_yyyy);
}

bool _Board::getDateTime(JsonDocument &docInput)
{
    uint32_t unixTime = rtc.getEpoch();
    
    docInput["Timestamp"] = unixTime;

    return true;
}