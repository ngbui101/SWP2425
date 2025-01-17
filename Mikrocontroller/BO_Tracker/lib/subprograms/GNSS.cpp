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
    if(!gpsModuleEnable){
        TurnOnGNSS();
    }
    if(!trackerModes.GnssMode){
        TurnOff();
    }
    // GNSS-Position und Genauigkeit abrufen
    if (GetGnssJsonPositionInformation(docInput, gnssData.startMillis))
    {
        getFirstFix = true;
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
            // runningLogger.logError("GetGNSSNMEASentences");
        }
        if (_BG96.GetGNSSNMEASentences(GPGSV, gnssData.gsv))
        {
            
            docInput["GSV"] = gnssData.gsv;
        }else{
            // runningLogger.logError("GetGNSSNMEASentences");
        }
    }
}

bool GNSS::TurnOff()
{
    if (_BG96.TurnOffGNSS())
    {   
        gpsModuleEnable = false;
        getFirstFix = false;
        gnssData.startMillis = 0;
        return true;
    }
    return false;
}

bool GNSS::TurnOnGNSS()
{
    if (_BG96.TurnOnGNSS(gnssData.workMode, WRITE_MODE))
    {
        gpsModuleEnable = true;
        gnssData.startMillis = millis();
        return true;
    }
    initLogger.logError("TurnOnGNSS");
    gpsModuleEnable = false;
    return false;
}

bool GNSS::isGnssModuleEnable(){
    return gpsModuleEnable;
}

bool GNSS::GetGnssJsonPositionInformation(JsonDocument &json, unsigned long starttime)
{
    char position[256];
    float accuracy;
    if (!_BG96.GetGNSSPositionInformation(position))
    {   
        json["gnss"] = "NoFix";
        return false;
    }
    _BG96.GetEstimationError(accuracy);
    position[sizeof(position) - 1] = '\0';

    const char *delimiter = ",";
    char *token = strtok(position, delimiter);
    int fieldIndex = 0;

    float latitude = 0.0f;
    float longitude = 0.0f;
    float hdop = 0.0f;
    int nsat = 0;

    while (token != nullptr)
    {
        switch (fieldIndex)
        {
        case 1:
            latitude = atof(token);
            break;
        case 2:
            longitude = atof(token);
            break;
        case 3:
            hdop = atof(token);
            break;
        case 10:
            nsat = atoi(token);
            break;
        default:
            break;
        }
        fieldIndex++;
        token = strtok(nullptr, delimiter);
    }

    if(TTFF == 0){
        currentTime = millis();
        TTFF = currentTime - starttime;
    }
    // Erstellen des verschachtelten "gnss" Objekts
    JsonObject gnss = json["gnss"].to<JsonObject>();
    gnss["latitude"] = latitude;
    gnss["longitude"] = longitude;
    gnss["hdop"] = hdop;
    gnss["nsat"] = nsat;
    gnss["accuracy"] = accuracy;
    gnss["TTFF"] = TTFF;
    return true;
}