#ifndef __GNSS
#define __GNSS

#include "BG96_GNSS.h"
#include <ArduinoJson.h>
#include <Arduino.h>
#include "MQTT_AWS.hpp"

#define DSerial SerialUSB
#define ATSerial Serial1

char gnss_posi[128];
float accuracy = 0.0;
char gnss_gsa[128];
char gnss_gsv[516];
//
unsigned long gnssStartMillis = 0;
unsigned long timeToFirstFix = 0;
GNSS_Work_Mode_t mode = MS_BASED;
bool gnssIsOn = false;

_BG96_GNSS _GNSS(ATSerial, DSerial);

bool InitGNSS()
{
    char currentTimestamp[64];

    _GNSS.GetLatestGMTTime(currentTimestamp);

    if (_GNSS.InitGpsOneXTRA(currentTimestamp))
    {
        DSerial.println("\r\nInit GpsOneXTRA Success!");
    }

    if (_GNSS.SetGNSSEnableNMEASentences(true))
    {
        DSerial.println("\r\nEnable NMEA Sentences Success!");
    }
    return true;
}
void handleGNSSMode(JsonDocument &docInput)
{
    if (!gnssIsOn && _GNSS.TurnOnGNSS(mode, WRITE_MODE))
    {
        gnssIsOn = true;
        gnssStartMillis = millis();
    }

    if (_GNSS.GetGNSSPositionInformation(gnss_posi) && _GNSS.GetEstimationError(accuracy))
    {
        timeToFirstFix = timeToFirstFix ? timeToFirstFix : millis() - gnssStartMillis;
        docInput["TimeToGetFirstFix"] = timeToFirstFix;
        docInput["Position"] = gnss_posi;
        docInput["Accuracy"] = accuracy;
        if (NmeaMode)
        {
            if (_GNSS.GetGNSSNMEASentences(GPGSA, gnss_gsa))
            {
                docInput["GSA"] = gnss_gsa;
            }
            else
            {
                DSerial.println("Failed to retrieve GSA NMEA Sentence.");
            }

            if (_GNSS.GetGNSSNMEASentences(GPGSV, gnss_gsv))
            {
                docInput["GSV"] = gnss_gsv;
            }
            else
            {
                DSerial.println("Failed to retrieve GSV NMEA Sentence.");
            }
        }
    }
    else
    {
        DSerial.println("Failed to retrieve GNSS Position or Accuracy.");
    }
}
#endif