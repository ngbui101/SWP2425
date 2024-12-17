#include "BG96_GNSS.h"

// Konstruktor ohne Parameter
/**
 * @brief Standardkonstruktor der Klasse _BG96_GNSS.
 *
 * Initialisiert die GNSS-Komponente ohne zusätzliche Parameter.
 */
_BG96_GNSS::_BG96_GNSS()
{
}

// Destruktor
/**
 * @brief Destruktor der Klasse _BG96_GNSS.
 *
 * Bereinigt die GNSS-Komponente.
 */
_BG96_GNSS::~_BG96_GNSS()
{
}

// Konstruktor mit Parametern für die seriellen Schnittstellen, der die Basisklasse _BG96_HTTP initialisiert
/**
 * @brief Konstruktor der Klasse _BG96_GNSS mit Initialisierung der seriellen Schnittstellen.
 *
 * @param atserial AT-Befehls-Schnittstelle.
 * @param dserial Daten-Schnittstelle für die Kommunikation.
 */
_BG96_GNSS::_BG96_GNSS(Stream &atserial, Stream &dserial) : _BG96_MQTT(atserial, dserial)
{
}

// Funktion zum Setzen der GNSS-Konstellation
/**
 * @brief Setzt die GNSS-Konstellation (GPS, GLONASS, BeiDou etc.).
 *
 * Verwendet den AT-Befehl AT+QGPSCFG="gnssconfig", um die unterstützten GNSS-Konstellationen zu konfigurieren.
 * @param constellation GNSS_Constellation_t Typ für die Auswahl der Konstellation.
 * @return true bei erfolgreicher Konfiguration, false bei Fehler.
 * @see Quectel BG96 GNSS AT Commands Manual, Abschnitt 2.1.8 (Seite 14)
 */
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
/**
 * @brief Aktiviert oder deaktiviert den automatischen GNSS-Start.
 *
 * Verwendet den AT-Befehl AT+QGPSCFG="autogps", um den GNSS-Modus auf automatisch oder manuell zu setzen.
 * @param auto_run true, um den automatischen Start zu aktivieren, false, um ihn zu deaktivieren.
 * @return true bei erfolgreicher Konfiguration, false bei Fehler.
 * @see Quectel BG96 GNSS AT Commands Manual, Abschnitt 2.1.9 (Seite 15)
 */
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
/**
 * @brief Aktiviert oder deaktiviert die Ausgabe von NMEA-Sätzen.
 *
 * Verwendet den AT-Befehl AT+QGPSCFG="nmeasrc", um festzulegen, ob NMEA-Sätze über den GNSS ausgegeben werden.
 * @param enable true, um die NMEA-Sätze zu aktivieren, false, um sie zu deaktivieren.
 * @return true bei Erfolg, false bei Fehler.
 * @see Quectel BG96 GNSS AT Commands Manual, Abschnitt 2.1.2 (Seite 10)
 */
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
/**
 * @brief Schaltet den GNSS ein.
 *
 * Verwendet den AT-Befehl AT+QGPS, um GNSS im ausgewählten Modus zu aktivieren.
 * @param mode GNSS_Work_Mode_t für den gewünschten Arbeitsmodus des GNSS.
 * @param status Modus (Lesen oder Schreiben) für den Befehl.
 * @return true bei Erfolg, false bei Fehler.
 * @see Quectel BG96 GNSS AT Commands Manual, Abschnitt 2.3 (Seite 17)
 */
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
/**
 * @brief Schaltet den GNSS aus.
 *
 * Verwendet den AT-Befehl AT+QGPSEND, um den GNSS zu deaktivieren.
 * @return true bei Erfolg, false bei Fehler.
 * @see Quectel BG96 GNSS AT Commands Manual, Abschnitt 2.4 (Seite 18)
 */
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
/**
 * @brief Ruft GNSS-Positionsdaten ab.
 *
 * Verwendet den AT-Befehl AT+QGPSLOC, um die aktuelle Position basierend auf den GNSS-Daten zu erhalten.
 * @param position Puffer, in den die Position geschrieben wird.
 * @return true bei Erfolg, false bei Fehler.
 * @see Quectel BG96 GNSS AT Commands Manual, Abschnitt 2.5 (Seite 19)
 */
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

bool _BG96_GNSS::GetGnssJsonPositionInformation(JsonDocument &json, unsigned long starttime)
{
    char position[256];
    float accuracy;
    if (!GetGNSSPositionInformation(position))
    {
        return false;
    }
    GetEstimationError(accuracy);
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

    // int parsedItems = sscanf(position, "%*f,%f,%f,%f,%*f,%*d,%*f,%*f,%*f,%*d,%d",
    //                          &latitude, &longitude, &hdop, &nsat);

    // if (parsedItems < 4)
    // {
    //     return false;
    // }
    unsigned long currentTime = millis();
    unsigned long TTFF = currentTime - starttime;
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

// Funktion zum Abrufen der GNSS-NMEA-Sätze
/**
 * @brief Ruft die NMEA-Sätze des GNSS ab.
 *
 * Verwendet den AT-Befehl AT+QGPSGNMEA, um spezifische NMEA-Daten wie GPGGA, GPRMC usw. zu erhalten.
 * @param type NMEA-Satz-Typ (z. B. GPGGA, GPRMC).
 * @param sentences Puffer, in den die Sätze geschrieben werden.
 * @return true bei Erfolg, false bei Fehler.
 * @see Quectel BG96 GNSS AT Commands Manual, Abschnitt 2.6 (Seite 21)
 */
bool _BG96_GNSS::GetGNSSNMEASentences(NMEA_Type_t type, char *sentences)
{
    char cmd[32];
    strcpy(cmd, GNSS_ACQUIRE_NMEA);
    switch (type)
    {
    case GPGGA:
        strcat(cmd, "=\"GGA\"");
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
/**
 * @brief Konfiguriert den Ausgabeport für GNSS-Daten.
 *
 * Verwendet den AT-Befehl AT+QGPSCFG="outport", um den Ausgabeport für NMEA-Sätze zu setzen.
 * @param outport GNSS_OutputPort_t für den gewünschten Ausgang (USB oder UART).
 * @return true bei Erfolg, false bei Fehler.
 * @see Quectel BG96 GNSS AT Commands Manual, Abschnitt 2.1.1 (Seite 9)
 */
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
// Funktion zur Initialisierung von gpsOneXTRA
/**
 * @brief Initialisiert die gpsOneXTRA-Funktion, um GNSS-Daten zu verbessern.
 *
 * Diese Methode aktiviert gpsOneXTRA und überprüft, ob die gpsOneXTRA-Daten aktuell sind.
 * Falls nicht, werden die Daten aktualisiert.
 * @return true, wenn gpsOneXTRA erfolgreich initialisiert wurde, false bei Fehler.
 * @see Quectel BG96 GNSS AT Commands Manual, Abschnitt 2.8.1 (Seite 24)
 */
bool _BG96_GNSS::InitGpsOneXTRA(char *currentTimestamp)
{
    if (!EnableGpsOneXTRA(ENABLE, READ_MODE))
    {
        if (!EnableGpsOneXTRA(ENABLE, WRITE_MODE))
        {
            return false;
        }
    }

    ResetModule();

    if (!InjectGpsOneXTRAData("UFS:xtra2.bin", READ_MODE, currentTimestamp))
    {
        // delete GPSOneXtra data
        TurnOffGNSS();
        DeleteAssistanceData(XTRA);
        if (!UpdateGpsOneXtraData(currentTimestamp))
        {
            return false;
        };
    };
    return true;
}
// Funktion zur Aktivierung von gpsOneXTRA
/**
 * @brief Aktiviert gpsOneXTRA, um Unterstützung durch externe GNSS-Daten zu bieten.
 *
 * Diese Methode verwendet den AT-Befehl AT+QGPSXTRA, um gpsOneXTRA zu aktivieren.
 * @return true bei erfolgreicher Aktivierung, false bei Fehler.
 * @see Quectel BG96 GNSS AT Commands Manual, Abschnitt 2.8.1 (Seite 24)
 */
bool _BG96_GNSS::EnableGpsOneXTRA(GNSS_Xtra_Enable_t mode, Cmd_Status_t status)
{
    char cmd[16], buf[8];
    strcpy(cmd, GNSS_ENABLE_GPSONEXTRA);
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

// Funktion zum Injizieren der gpsOneXTRA-Zeit
/**
 * @brief Injiziert die aktuelle Zeit in das GNSS-System zur Verbesserung der gpsOneXTRA-Daten.
 *
 * Verwendet den AT-Befehl AT+QGPSXTRATIME, um die aktuelle Zeit für gpsOneXTRA-Daten zu injizieren.
 * @param time Zeitstempel im UTC-Format.
 * @return true bei Erfolg, false bei Fehler.
 * @see Quectel BG96 GNSS AT Commands Manual, Abschnitt 2.8.2 (Seite 25)
 */
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

// Funktion zum Injizieren der gpsOneXTRA-Daten
/**
 * @brief Injiziert gpsOneXTRA-Daten in das GNSS-System, um die Zeit bis zum ersten Fix zu verbessern.
 *
 * Diese Methode verwendet den AT-Befehl AT+QGPSXTRADATA, um die Daten in das System zu injizieren.
 * @param filename Name der Datei, die die gpsOneXTRA-Daten enthält.
 * @return true bei Erfolg, false bei Fehler.
 * @see Quectel BG96 GNSS AT Commands Manual, Abschnitt 2.8.3 (Seite 26)
 */
bool _BG96_GNSS::InjectGpsOneXTRAData(const char *filename, Cmd_Status_t status, char *currentTimestamp)
{
    char cmd[16], buf[8];
    strcpy(cmd, GNSS_INJECT_GPSONEXTRA_DATA);

    if (status == READ_MODE)
    {
        strcat(cmd, "?");
        if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2))
        {
            char *sta_buf = searchStrBuffer(": ");
            char *time_start = strchr(sta_buf, '\"');
            char *time_end = strchr(time_start + 1, '\"');
            *time_end = '\0';

            char gpsDataTimestamp[64];
            strcpy(gpsDataTimestamp, time_start + 1);

            time_t gpsTime = parseTimestamp(gpsDataTimestamp);
            time_t currentTime = parseTimestamp(currentTimestamp);

            double diffDays = difftime(currentTime, gpsTime) / (60 * 60 * 24);

            if (diffDays > 1)
            {
                // Wenn der Zeitstempel älter als 1 Tag ist, gib false zurück
                return false;
            }
            else
            {
                // Wenn der Zeitstempel innerhalb von 1 Tagen liegt, gib true zurück
                return true;
            }
        }
    }
    else if (status == WRITE_MODE)
    {
        InjectGpsOneXTRATime(currentTimestamp); // Set Timestamp

        sprintf(buf, "=\"%s\"", filename);
        strcat(cmd, buf);
        if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 10))
        {
            return true;
        }
    }

    return false;
}

// Funktion zur Aktualisierung der gpsOneXTRA-Daten
/**
 * @brief Aktualisiert die gpsOneXTRA-Daten durch Herunterladen und Injizieren der neuen Daten.
 *
 * Diese Methode lädt die aktuellen gpsOneXTRA-Daten von einem vorgegebenen Server herunter
 * und injiziert sie in das GNSS-System.
 * @return true bei erfolgreicher Aktualisierung, false bei Fehler.
 * @see Quectel BG96 GNSS AT Commands Manual, Abschnitt 2.8.4 (Seite 27)
 */

bool _BG96_GNSS::UpdateGpsOneXtraData(char *currentTimestamp)
{
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
                    if (EnableGpsOneXTRA(ENABLE, WRITE_MODE))
                    {
                        if (InjectGpsOneXTRAData("UFS:xtra2.bin", WRITE_MODE, currentTimestamp))
                        {
                            return true; // Erfolgreich, Funktion beenden
                        }
                    }
                }
            }
        }
    }

    // Wenn keine Links funktioniert haben, gib Fehler zurück
    return false;
}

bool _BG96_GNSS::DeleteAssistanceData(GNSS_Delete_t deletetype)
{
    char cmd[64];
    strcpy(cmd, GNSS_DELETE_ASSISTANCE_DATA);
    if (deletetype == ALLNOXTRA)
    {
        strcat(cmd, "=0");
    }
    else if (deletetype == NONEHOTSTART)
    {
        strcat(cmd, "=1");
    }
    else if (deletetype == SOMEWARMSTART)
    {
        strcat(cmd, "=2");
    }
    else if (deletetype == XTRA)
    {
        strcat(cmd, "=3");
    }
    if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2))
    {
        return true;
    }
    return false;
}

bool _BG96_GNSS::GetEstimationError(float &accuracy)
{
    accuracy = 0.0;
    char cmd[64];
    strcpy(cmd, GNSS_CONFIGURATION);
    strcat(cmd, "=\"estimation_error\"");
    if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 10))
    {
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        char *sta_buf = searchStrBuffer(": ");
        if (sta_buf)
        {
            sta_buf += 2;
            char *token = strtok(sta_buf, ",");
            int index = 0;
            float hori_unc = 0.0, vert_unc = 0.0;

            while (token != nullptr)
            {
                if (index == 1)
                {
                    hori_unc = atof(token);
                }
                else if (index == 2)
                {
                    vert_unc = atof(token);
                    break;
                }
                token = strtok(nullptr, ",");
                index++;
            }
            // accuracy = sqrt(pow(hori_unc, 2) + pow(vert_unc, 2));
            accuracy = hori_unc;
            return true;
        }
    }
    return false;
}

/**
 * @brief Fügt einen neuen Geofence-Bereich hinzu.
 *
 * Diese Methode verwendet den AT-Befehl AT+QCFGEXT="addgeo" und konfiguriert den Geofence mit den angegebenen Parametern.
 *
 * @param geoID Eindeutige ID des Geofence.
 * @param mode Betriebsmodus des Geofence.
 * @param shape Form des Geofence (z. B. Kreis).
 * @param latituide Breitengrad des Mittelpunkts des Geofence (-90 bis 90).
 * @param longituide Längengrad des Mittelpunkts des Geofence (-180 bis 180).
 * @param radius Radius des Geofence in Metern (muss positiv sein).
 * @return true, wenn der Geofence erfolgreich hinzugefügt wurde; andernfalls false.
 */
bool _BG96_GNSS::AddGeoFence(unsigned int geoID, GEOFENCE_MODE_t mode, GEOFENCE_SHAPE_t shape, float latituide, float longituide, unsigned int radius)
{
    // Überprüfen Sie die Gültigkeit der Parameterwerte
    if (latituide < -90.0 || latituide > 90.0 || longituide < -180.0 || longituide > 180.0 || radius <= 0)
    {
        return false;
    }
    char cmd[64];
    sprintf(cmd, "%s=\"addgeo\",%d,%d,%d,%f,%f,%d", GNSS_GEO_FENCE, geoID, mode, shape, latituide, longituide, radius);
    if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 3))
    {
        return true;
    }
    return false;
}

/**
 * @brief Überprüft, ob die aktuelle Position des Geräts innerhalb oder außerhalb eines definierten Geofences liegt.
 *
 * Diese Methode sendet den AT-Befehl `AT+QCFGEXT="querygeo",<geoID>` an das BG96-GNSS-Modul, um den Status eines bestimmten Geofence-Bereichs abzufragen.
 * Das Modul antwortet mit der Position des Geräts relativ zu dem angegebenen Geofence (`<posWrtGeofence>`),
 * die dann analysiert wird, um festzustellen, ob das Gerät innerhalb oder außerhalb des Geofence ist.
 *
 * Erwartetes Antwortformat:
 * `+QCFGEXT: "querygeo",<geoID>,<posWrtGeofence>`
 *
 * - `<geoID>`: Geofence-ID (entspricht dem `geoID`-Parameter), um den relevanten Geofence zu identifizieren.
 * - `<posWrtGeofence>`: Integer-Wert, der die Position relativ zum Geofence beschreibt:
 *     - 0: Position unbekannt
 *     - 1: Position ist innerhalb des Geofence
 *     - 2: Position ist außerhalb des Geofence
 *
 * @param geoID Die eindeutige ID des Geofence, für den der Status abgefragt wird.
 * @return `true`, wenn sich die Position innerhalb des Geofence befindet (`<posWrtGeofence>` ist 1).
 *         `false`, wenn sich die Position außerhalb des Geofence befindet oder ein Fehler auftritt.
 *
 * @note Diese Methode erfordert eine erfolgreiche Kommunikation mit dem BG96-GNSS-Modul und eine gültige Antwort.
 *       Falls keine gültige Antwort empfangen wird, gibt die Methode `false` zurück.
 *
 * @see Quectel BG96 GNSS AT Commands Manual, Abschnitt 2.7 für weitere Informationen zu Geofencing-Kommandos.
 */
bool _BG96_GNSS::QueryGeoFence(unsigned int geoID)
{
    char cmd[64];
    sprintf(cmd, "%s=\"querygeo\",%d", GNSS_GEO_FENCE, geoID);

    if (sendAndSearch(cmd, "+QCFGEXT: \"querygeo\"", RESPONSE_ERROR, 5))
    {
        // Erwartete Antwort: +QCFGEXT: "querygeo",<geoID>,<posWrtGeofence>

        char *sta_buf = searchStrBuffer(": \"querygeo\",");
        if (sta_buf != nullptr)
        {
            char *posStr = strchr(sta_buf, ','); // Sucht erstes Komma
            if (posStr != nullptr)
            {
                posStr = strchr(posStr + 1, ','); // Sucht zweites Komma, in dem die Position gespeichert ist
                if (posStr != nullptr)
                {
                    int posWrtGeofence = atoi(posStr + 1);

                    // Rückgabe basierend auf der Position relativ zum Geofence
                    if (posWrtGeofence == 1)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

GEOFENCE_STATUS_t _BG96_GNSS::getGeoFencingStatus(unsigned int geoID)
{
    char position[128];
    if (!TurnOnGNSS(MS_BASED, READ_MODE))
    {
        TurnOnGNSS(MS_BASED, WRITE_MODE);
    }
    if (GetGNSSPositionInformation(position))
    {
        if (QueryGeoFence(geoID))
        {
            return INSIDE_GEOFENCE;
        }
        else
        {
            return OUTSIDE_GEOFENCE;
        }
        TurnOffGNSS();
    }
    else
    {
        return NOFIX;
    }
}