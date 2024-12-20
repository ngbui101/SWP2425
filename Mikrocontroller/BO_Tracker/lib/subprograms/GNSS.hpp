#ifndef __GNSS
#define __GNSS

#include "MQTT_AWS.hpp"

struct GNSS_Tracker {
    char gsa[128];
    char gsv[516];
    unsigned long startMillis = 0;
    GNSS_Work_Mode_t workMode = MS_BASED;
    bool isOn = false;
    unsigned int geoID = 1;
} gnssTracker;


bool InitGNSS(Stream &DSerial, _BG96_GNSS &_GNSS) {
    char currentTimestamp[64];
    _GNSS.GetLatestGMTTime(currentTimestamp);

    if (!_GNSS.InitGpsOneXTRA(currentTimestamp)) {
        DSerial.println("\r\nError: Init GpsOneXTRA fail!");
    }
    if(!_GNSS.InitAGPS(SUPURL, APN)){
        DSerial.println("\r\nError: Init AGPS fail");
    }
    if (!_GNSS.SetGNSSEnableNMEASentences(true)) {
        DSerial.println("\r\nError: SetEnableNMEASentences fail");
    }

    return true;
}

void GPSOneXtraCheckForUpdate(Stream &DSerial, _BG96_GNSS &_GNSS) {
    char currentTimestamp[64];
    _GNSS.GetLatestGMTTime(currentTimestamp);
    _GNSS.InjectGpsOneXTRAData("UFS:xtra2.bin", WRITE_MODE, currentTimestamp);
}

void handleGNSSMode(Stream &DSerial, _BG96_GNSS &_GNSS, JsonDocument &docInput) {
    // GNSS einschalten, falls noch nicht aktiviert
    if (!gnssTracker.isOn && _GNSS.TurnOnGNSS(gnssTracker.workMode, WRITE_MODE)) {
        gnssTracker.isOn = true;
        gnssTracker.startMillis = millis();
    }

    // GNSS-Position und Genauigkeit abrufen
    if (_GNSS.GetGnssJsonPositionInformation(docInput, gnssTracker.startMillis)) {
        
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

boolean addGeo(Stream &DSerial, _BG96_GNSS &_GNSS){

    if(_GNSS.AddGeoFence(gnssTracker.geoID,DISABLE_GEOFENCE,CIRLE,trackerModes.geoLatitude,trackerModes.geoLongitude,trackerModes.geoRadius)){
        DSerial.println("GeoFence added successfully");
        return true;
    }
    return false;
}
#endif
