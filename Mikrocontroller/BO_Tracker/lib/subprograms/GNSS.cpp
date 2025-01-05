#include "GNSS.h"

// Konstruktor der Klasse GNSS
GNSS::GNSS(Stream &serial, _BG96_GNSS &gnssModule, JsonDocument &doc) 
    : DSerial(serial), _GNSS(gnssModule), docInput(doc) {}

// Initialisiert GNSS
bool GNSS::InitGNSS() {
    char currentTimestamp[64];
    _GNSS.GetLatestGMTTime(currentTimestamp);

    if (!_GNSS.InitGpsOneXTRA(currentTimestamp)) {
        DSerial.println("\r\nError: Init GpsOneXTRA fail!");
    }
    if (!_GNSS.InitAGPS(SUPURL, APN)) {
        DSerial.println("\r\nError: Init AGPS fail");
    }
    if (!_GNSS.SetGNSSEnableNMEASentences(true)) {
        DSerial.println("\r\nError: SetEnableNMEASentences fail");
    }

    return true;
}

// Prüft auf Updates für GPSOneXtra-Daten
void GNSS::GPSOneXtraCheckForUpdate() {
    char currentTimestamp[64];
    _GNSS.GetLatestGMTTime(currentTimestamp);
    _GNSS.InjectGpsOneXTRAData("UFS:xtra2.bin", WRITE_MODE, currentTimestamp);
}

// Handhabt GNSS-Modus, aktiviert GNSS und liest Position
void GNSS::handleGNSSMode() {
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

// Fügt Geofencing hinzu
bool GNSS::addGeo() {
    if (_GNSS.AddGeoFence(gnssTracker.geoID, DISABLE_GEOFENCE, CIRLE, trackerModes.geoLatitude, trackerModes.geoLongitude, trackerModes.geoRadius)) {
        DSerial.println("GeoFence added successfully");
        return true;
    }
    return false;
}
