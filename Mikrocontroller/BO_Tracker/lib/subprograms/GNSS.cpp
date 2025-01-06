#include "GNSS.h"

// Konstruktor der Klasse GNSS
GNSS::GNSS(Stream &atSerial, Stream &dSerial, JsonDocument &doc)
    : HTTP(atSerial, dSerial), docInput(doc) {}

// Initialisiert GNSS
bool GNSS::InitGNSS()
{
    char currentTimestamp[64];
    _BG96.GetLatestGMTTime(currentTimestamp);

    if (!_BG96.InitGpsOneXTRA(currentTimestamp))
    {
        initLogger.logError("GPS_OneXTRA");
        return false;
    }

    if (!_BG96.InitAGPS(SUPURL, APN))
    {
        initLogger.logError("AGPS");
        return false;
    }

    if (!_BG96.SetGNSSEnableNMEASentences(true))
    {
        initLogger.logError("Enable_NMEA");
        return false;
    }
    return true;
}

// Prüft auf Updates für GPSOneXTRA-Daten
void GNSS::GPSOneXtraCheckForUpdate()
{
    char currentTimestamp[64];
    _BG96.GetLatestGMTTime(currentTimestamp);
    _BG96.InjectGpsOneXTRAData("UFS:xtra2.bin", WRITE_MODE, currentTimestamp);
}

// Handhabt GNSS-Modus
void GNSS::handleGNSSMode()
{
    // GNSS einschalten, falls es deaktiviert ist
    if (!gpsModuleEnable && _BG96.TurnOnGNSS(gnssData.workMode, WRITE_MODE))
    {
        gpsModuleEnable = true;
        gnssData.startMillis = millis();
    }

    // GNSS-Position und Genauigkeit abrufen
    if (_BG96.GetGnssJsonPositionInformation(docInput, gnssData.startMillis))
    {
        DSerial.println("GNSS-Positionsdaten erfolgreich abgerufen.");
    }
    else
    {
        DSerial.println("Fehler beim Abrufen der GNSS-Position.");
    }

    // NMEA-Sätze abrufen, falls aktiviert
    if (trackerModes.NmeaMode)
    {
        if (_BG96.GetGNSSNMEASentences(GPGSA, gnssData.gsa))
        {
            docInput["GSA"] = gnssData.gsa;
        }
        else
        {
            DSerial.println("Fehler: GSA-NMEA-Satz konnte nicht abgerufen werden.");
        }

        if (_BG96.GetGNSSNMEASentences(GPGSV, gnssData.gsv))
        {
            docInput["GSV"] = gnssData.gsv;
        }
        else
        {
            DSerial.println("Fehler: GSV-NMEA-Satz konnte nicht abgerufen werden.");
        }
    }
}

// Fügt Geofencing hinzu
bool GNSS::addGeo()
{
    if (_BG96.AddGeoFence(gnssData.geoID, DISABLE_GEOFENCE, CIRLE, trackerModes.geoLatitude, trackerModes.geoLongitude, trackerModes.geoRadius))
    {
        DSerial.println("Geofence erfolgreich hinzugefügt.");
        return true;
    }
    DSerial.println("Fehler beim Hinzufügen des Geofences.");
    return false;
}

bool GNSS::TurnOff()
{
    if (_BG96.TurnOffGNSS())
    {
        gpsModuleEnable = false;
        gnssData.startMillis = 0;
        return true;
    }
    return false;
}