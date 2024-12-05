#include "Board.h"

_Board::_Board()
{
}
bool _Board::initBoard()
{
    if (initBattery() && initTemp())
        return true;
    else
        return false;
}

bool _Board::setupRTCFromModem(const char* modemTime) {
    uint16_t year;
    uint8_t month, day, hour, minute, second;

    if (sscanf(modemTime, "%4hu/%2hhu/%2hhu,%2hhu:%2hhu:%2hhu", &year, &month, &day, &hour, &minute, &second) == 6) {
        // RTC konfigurieren
        rtc.setYear(year);
        rtc.setMonth(month);
        rtc.setDay(day);
        rtc.setHours(hour);
        rtc.setMinutes(minute);
        rtc.setSeconds(second);
        return true; 
    }

    return false; 
}

char* _Board::getDateTime() {
    uint32_t unixTime = rtc.getEpoch();

    static char buffer[11]; 

    snprintf(buffer, sizeof(buffer), "%lu", unixTime);

    return buffer;
}
