#ifndef __GNSS
#define __GNSS

#include "BG96_GNSS.h"
#include <ArduinoJson.h>
#include <Arduino.h>
#include "MQTT_AWS.hpp"

#define DSerial SerialUSB
#define ATSerial Serial1

// GNSS Tracker Konfiguration und Zustand
struct GNSS_Tracker {
    char position[128];
    bool geoFenceInit = false;
    float accuracy = 0.0;
    char gsa[128];
    char gsv[516];
    unsigned long startMillis = 0;
    unsigned long timeToFirstFix = 0;
    GNSS_Work_Mode_t workMode = MS_BASED;
    bool isOn = false;
    unsigned int geoID = 1;
} gnssTracker;

_BG96_GNSS _GNSS(ATSerial, DSerial);

bool InitGNSS() {
    char currentTimestamp[64];
    _GNSS.GetLatestGMTTime(currentTimestamp);

    if (_GNSS.InitGpsOneXTRA(currentTimestamp)) {
        DSerial.println("\r\nInit GpsOneXTRA Success!");
    }

    if (_GNSS.SetGNSSEnableNMEASentences(true)) {
        DSerial.println("\r\nEnable NMEA Sentences Success!");
    }
    return true;
}

void GPSOneXtraCheckForUpdate() {
    char currentTimestamp[64];
    _GNSS.GetLatestGMTTime(currentTimestamp);
    _GNSS.InjectGpsOneXTRAData("UFS:xtra2.bin", WRITE_MODE, currentTimestamp);
}

void handleGNSSMode(JsonDocument &docInput) {
    // GNSS einschalten, falls noch nicht aktiviert
    if (!gnssTracker.isOn && _GNSS.TurnOnGNSS(gnssTracker.workMode, WRITE_MODE)) {
        gnssTracker.isOn = true;
        gnssTracker.startMillis = millis();
    }

    // GNSS-Position und Genauigkeit abrufen
    if (_GNSS.GetGNSSPositionInformation(gnssTracker.position) && _GNSS.GetEstimationError(gnssTracker.accuracy)) {
        gnssTracker.timeToFirstFix = gnssTracker.timeToFirstFix ? gnssTracker.timeToFirstFix : millis() - gnssTracker.startMillis;
        docInput["TimeToGetFirstFix"] = gnssTracker.timeToFirstFix;
        docInput["Position"] = gnssTracker.position;
        docInput["Accuracy"] = gnssTracker.accuracy;
    } else {
        DSerial.println("Failed to retrieve GNSS Position or Accuracy.");
    }
    if (trackerModes.NmeaMode) {
        if (_GNSS.GetGNSSNMEASentences(GPGSA, gnssTracker.gsa)) {
            docInput["GSA"] = gnssTracker.gsa;
        } else {
            DSerial.println("Failed to retrieve GSA NMEA Sentence.");
        }

        if (_GNSS.GetGNSSNMEASentences(GPGSV, gnssTracker.gsv)) {
            docInput["GSV"] = gnssTracker.gsv;
        } else {
            DSerial.println("Failed to retrieve GSV NMEA Sentence.");
        }
    }
}

boolean addGeo(){
    if(_GNSS.AddGeoFence(gnssTracker.geoID,DISABLE_GEOFENCE,CIRLE,trackerModes.geoLatitude,trackerModes.geoLongitude,trackerModes.geoRadius)){
        gnssTracker.geoFenceInit = true;
        return true;
    }
    return false;
}
#endif
