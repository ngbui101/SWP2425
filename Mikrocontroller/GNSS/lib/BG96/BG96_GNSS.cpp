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
_BG96_GNSS::_BG96_GNSS(Stream &atserial, Stream &dserial) : _BG96_HTTP(atserial, dserial)
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
    if(!EnableGpsOneXTRA(ENABLE,READ_MODE)){
        if(!EnableGpsOneXTRA(ENABLE,WRITE_MODE)){
            return false;
        }
    }

    ResetModule();

    if(!InjectGpsOneXTRAData("UFS:xtra2.bin", READ_MODE, currentTimestamp)){
        //delete GPSOneXtra data
        TurnOffGNSS();
        DeleteAssistanceData(XTRA);
        if(!UpdateGpsOneXtraData(currentTimestamp)){
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
            // Extrahiere die Antwort, z.B. "+QGPSXTRADATA: 10080,\"2024/10/05,08:00:00\""
            char *sta_buf = searchStrBuffer(": ");         // Sucht den Beginn der Daten
            char *time_start = strchr(sta_buf, '\"');      // Sucht nach dem ersten Anführungszeichen
            char *time_end = strchr(time_start + 1, '\"'); // Sucht nach dem zweiten Anführungszeichen
            *time_end = '\0';                              // Beendet den String nach dem Zeitstempel
            // Extrahiere den Zeitstempel (z.B. "2024/10/05,08:00:00")
            char gpsDataTimestamp[64];
            strcpy(gpsDataTimestamp, time_start + 1); // Kopiert den Zeitstempel ohne Anführungszeichen

            // Berechne die Differenz zwischen dem gpsDataTimestamp und currentTimestamp
            // Verwandle den String-Zeitstempel in ein Unix-Zeitformat
            time_t gpsTime = parseTimestamp(gpsDataTimestamp);
            time_t currentTime = parseTimestamp(currentTimestamp);

            // Berechne den Unterschied in Tagen (7 Tage = 7 * 24 * 60 * 60 Sekunden)
            double diffDays = difftime(currentTime, gpsTime) / (60 * 60 * 24);

            if (diffDays > 7)
            {
                // Wenn der Zeitstempel älter als 7 Tage ist, gib false zurück
                return true;
            }
            else
            {
                // Wenn der Zeitstempel innerhalb von 7 Tagen liegt, gib true zurück
                return false;
            }
        }
    }
    else if (status == WRITE_MODE)
    {
        InjectGpsOneXTRATime(currentTimestamp); //Set Timestamp

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
