#include "GNSS.h"

// Konstruktor der Klasse GNSS
GNSS::GNSS(Stream &atSerial, Stream &dSerial, JsonDocument &doc)
    : MQTT_AWS(atSerial, dSerial, doc) {}

// Initialisiert GNSS
bool GNSS::InitGNSS()
{
    char currentTimestamp[64];
    _BG96.GetLatestGMTTime(currentTimestamp);

    if (!_BG96.InitGpsOneXTRA(currentTimestamp))
    {
        DSerial.println("Fehler: Init GpsOneXTRA fehlgeschlagen!");
        return false;
    }

    if (!_BG96.InitAGPS(SUPURL, APN))
    {
        DSerial.println("Fehler: Init AGPS fehlgeschlagen!");
        return false;
    }

    if (!_BG96.SetGNSSEnableNMEASentences(true))
    {
        DSerial.println("Fehler: NMEA-Sätze konnten nicht aktiviert werden!");
        return false;
    }

    DSerial.println("GNSS erfolgreich initialisiert.");
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
    if (!gnssData.isOn && _BG96.TurnOnGNSS(gnssData.workMode, WRITE_MODE))
    {
        gnssData.isOn = true;
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
        gnssData.isOn = false;
        gnssData.startMillis = 0;
        return true;
    }
    return false;
}