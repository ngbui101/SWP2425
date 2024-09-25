#include "BG96_GNSS.h"

// Konstruktor ohne Parameter
_BG96_GNSS::_BG96_GNSS()
{
}

// Destruktor
_BG96_GNSS::~_BG96_GNSS()
{
}

// Konstruktor mit Parametern für die seriellen Schnittstellen, der die Basisklasse _BG96_Common initialisiert
_BG96_GNSS::_BG96_GNSS(Stream &atserial, Stream &dserial) : _BG96_HTTP(atserial, dserial)
{
}

// Funktion zum Setzen der GNSS-Konstellation
bool _BG96_GNSS::SetGNSSConstellation(GNSS_Constellation_t constellation)
{
    char cmd[32], buf[32];
    strcpy(cmd, GNSS_CONFIGURATION);
    sprintf(buf, "=\"gnssconfig\",%d", constellation);
    strcat(cmd, buf);
    if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2))
    {
        return true;
    }
    return false;
}

// Funktion zum automatischen Starten des GNSS
bool _BG96_GNSS::SetGNSSAutoRun(bool auto_run)
{
    char cmd[32];
    strcpy(cmd, GNSS_CONFIGURATION);
    if (auto_run)
    {
        strcat(cmd, "\"autogps\",1");
        if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2))
        {
            return true;
        }
    }
    else
    {
        strcat(cmd, "\"autogps\",0");
        if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2))
        {
            return true;
        }
    }
    return false;
}

// Funktion zum Aktivieren oder Deaktivieren der NMEA-Sätze
bool _BG96_GNSS::SetGNSSEnableNMEASentences(bool enable)
{
    char cmd[32];
    strcpy(cmd, GNSS_CONFIGURATION);
    if (enable)
    {
        strcat(cmd, "=\"nmeasrc\",1");
    }
    else
    {
        strcat(cmd, "=\"nmeasrc\",0");
    }
    if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2))
    {
        return true;
    }
    return false;
}

// Funktion zum Einschalten des GNSS
bool _BG96_GNSS::TurnOnGNSS(GNSS_Work_Mode_t mode, Cmd_Status_t status)
{
    char cmd[16], buf[8];
    strcpy(cmd, GNSS_TURN_ON);
    if (status == READ_MODE)
    {
        strcat(cmd, "?");
        if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2))
        {
            char *sta_buf = searchStrBuffer(": ");
            char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
            *end_buf = '\0';
            if (atoi(sta_buf + 2) == 1)
            {
                return true;
            }
        }
    }
    else if (status == WRITE_MODE)
    {
        sprintf(buf, "=%d", mode);
        strcat(cmd, buf);
        if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 10))
        {
            return true;
        }
    }
    return false;
}

// Funktion zum Ausschalten des GNSS
bool _BG96_GNSS::TurnOffGNSS()
{
    char cmd[16];
    strcpy(cmd, GNSS_TURN_OFF);
    if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 10))
    {
        return true;
    }
    return false;
}

// Funktion zum Abrufen der GNSS-Positionsinformationen
bool _BG96_GNSS::GetGNSSPositionInformation(char *position)
{
    char cmd[16];
    strcpy(cmd, GNSS_GET_POSITION);
    strcat(cmd, "=2");
    if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 10))
    {
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        char *sta_buf = searchStrBuffer(": ");
        strcpy(position, sta_buf + 2);
        return true;
    }
    return false;
}

// Funktion zum Abrufen der GNSS-NMEA-Sätze
bool _BG96_GNSS::GetGNSSNMEASentences(NMEA_Type_t type, char *sentences)
{
    char cmd[32];
    strcpy(cmd, GNSS_ACQUIRE_NMEA);
    switch (type)
    {
    case GPGGA:
        strcat(cmd, "=\"GCA\"");
        break;
    case GPRMC:
        strcat(cmd, "=\"RMC\"");
        break;
    case GPGSV:
        strcat(cmd, "=\"GSV\"");
        break;
    case GPGSA:
        strcat(cmd, "=\"GSA\"");
        break;
    case GPVTG:
        strcat(cmd, "=\"VTG\"");
        break;
    default:
        return false;
    }
    if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 10))
    {
        char *sta_buf = searchStrBuffer(": ");
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        strcpy(sentences, sta_buf + 2);
        return true;
    }
    return false;
}

// Funktion zum Setzen des GNSS-Ausgabeports
bool _BG96_GNSS::SetGNSSOutputPort(GNSS_OutputPort_t outport)
{
    char cmd[32];
    strcpy(cmd, GNSS_CONFIGURATION);
    if (outport == NOPORT)
    {
        strcat(cmd, "=\"outport\",\"none\"");
    }
    else if (outport == USBNMEA)
    {
        strcat(cmd, "=\"outport\",\"usbnmea\"");
    }
    else if (outport == UARTNMEA)
    {
        strcat(cmd, "=\"outport\",\"uartnmea\"");
    }
    if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2))
    {
        return true;
    }
    return false;
}
bool _BG96_GNSS::InitGpsOneXTRA()
{
    if (EnableGpsOneXTRA())
    {
        if (!IsGpsOneXtraDataUptoDate())
        {
            UpdateGpsOneXtraData();
        }
    }

    return false;
}

bool _BG96_GNSS::EnableGpsOneXTRA()
{
    char cmd[16];
    strcpy(cmd, GNSS_ENABLE_GPSONEXTRA);
    strcat(cmd, "=1"); // Enable gpsOneXTRA assistance
    if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 10))
    {
        return true;
    }
    return false;
}

bool _BG96_GNSS::InjectGpsOneXTRATime(const char *time)
{
    char cmd[64], buf[64];
    strcpy(cmd, GNSS_INJECT_GPSONEXTRA_TIME);
    sprintf(buf, "=0,\"%s\",%d,%d,%d", time, 1, 0, 5000);
    strcat(cmd, buf);
    if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 10))
    {
        return true;
    }
    return false;
}

bool _BG96_GNSS::InjectGpsOneXTRAData(const char *filename)
{
    char cmd[64];
    strcpy(cmd, GNSS_INJECT_GPSONEXTRA_DATA);
    sprintf(cmd + strlen(cmd), "=\"%s\"", filename); // Specify the filename
    if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 10))
    {
        return true;
    }
    return false;
}
bool _BG96_GNSS::IsGpsOneXtraDataUptoDate()
{
    char cmd[64];
    strcpy(cmd, GNSS_INJECT_GPSONEXTRA_DATA);
    strcat(cmd, "?");
    if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 10))
    {
        return false;
    }
    return true;
}

bool _BG96_GNSS::UpdateGpsOneXtraData()
{
    char currentTimestamp[64];
    if (!GetLatestGMTTime(currentTimestamp))
    {
        // Fehlerbehandlung: Konnte die aktuelle GMT-Zeit nicht abrufen
        return false;
    }

    const int link_count = sizeof(xtra_links) / sizeof(xtra_links[0]);
    // Versuch, jeden Link zu verwenden
    for (int i = 0; i < link_count; i++)
    {
        if (HTTPURL((char *)xtra_links[i], WRITE_MODE))
        {
            if (HTTPGET(80))
            {
                if (HTTPReadToFile("xtra2.bin", 80))
                {
                    if (EnableGpsOneXTRA())
                    {
                        if (InjectGpsOneXTRATime(currentTimestamp))
                        {
                            if (InjectGpsOneXTRAData("UFS:xtra2.bin"))
                            {
                                return true; // Erfolgreich, Funktion beenden
                            }
                        }
                    }
                }
            }
        }
    }

    // Wenn keine Links funktioniert haben, gib Fehler zurück
    return false;
}
