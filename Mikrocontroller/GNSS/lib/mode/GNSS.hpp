#ifndef __GNSS
#define __GNSS

#include "BG96_GNSS.h"

bool InitGNSS(_BG96_GNSS &GNSS,
              Stream &DSerial,
              char *currentTimestamp)
{
    GNSS_Work_Mode_t mode = STAND_ALONE;

    if (GNSS.InitGpsOneXTRA(currentTimestamp))
    {
        DSerial.println("\r\nInit GpsOneXTRA Success!");
    }

    while (!GNSS.TurnOnGNSS(mode, WRITE_MODE))
    {
        DSerial.println("\r\nOpen the GNSS Function Fali!");
        if (GNSS.TurnOnGNSS(mode, READ_MODE))
        {
            DSerial.println("\r\nThe GNSS Function is Opened!");
            GNSS.TurnOffGNSS();
        }
    }
    DSerial.println("\r\nOpen the GNSS Function Success!");
    return true;
}
#endif