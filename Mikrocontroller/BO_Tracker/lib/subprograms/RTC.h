#ifndef _RTC_H_
#define _RTC_H_

#include "Temperature.h"
#include <RTCZero.h>

class _RTC : public _Temperature
{
public:
    _RTC();
    ~_RTC();

    bool setcurrentTime(const char *modemTime);

    char *getDateTime();

    bool enableAlarm(unsigned long millis);

    bool enableAlarm(uint8_t *day, uint8_t *hour,
                     uint8_t *minute,
                     uint8_t *second);

    void calculateTimeForAlarm(uint8_t *day, uint8_t *hour,
                               uint8_t *minute,
                               uint8_t *second, unsigned long millisSpan);

private:
    RTCZero rtc;
    static void rtcCallback();
};

#endif // _RTC_H_
