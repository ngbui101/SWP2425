#ifndef _RTC_H_
#define _RTC_H_

#include "Temperature.h"
#include <RTCZero.h>

class _RTC : public _Temperature {
public:
    _RTC();
    ~_RTC();

    bool setupRTCFromModem(const char *modemTime);

    char *getDateTime();

private:
    RTCZero rtc;
};

#endif // _RTC_H_
