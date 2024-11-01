#ifndef __GNSS
#define __GNSS

#include "BG96_GNSS.h"

bool InitGNSS(_BG96_GNSS &GNSS,
              Stream &DSerial)
{   
    char currentTimestamp[64];

    GNSS.GetLatestGMTTime(currentTimestamp); 

    if (GNSS.InitGpsOneXTRA(currentTimestamp))
    {
        DSerial.println("\r\nInit GpsOneXTRA Success!");
    }
    
    if(GNSS.SetGNSSEnableNMEASentences(true)){
         DSerial.println("\r\nEnable NMEA Sentences Success!");
    }
    
    return true;
}
#endif