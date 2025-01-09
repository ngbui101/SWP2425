#include "BG96_Common.h"

/**
 * @brief Standardkonstruktor der Klasse _BG96_Common.
 *
 * Dieser Konstruktor initialisiert die Klasse ohne spezifische serielle Schnittstellen.
 */
_BG96_Common::_BG96_Common()
{
}

/**
 * @brief Destruktor der Klasse _BG96_Common.
 *
 * Der Destruktor führt keine spezifischen Aktionen aus.
 */
_BG96_Common::~_BG96_Common()
{
}

/**
 * @brief Parametrisierter Konstruktor der Klasse _BG96_Common.
 *
 * Dieser Konstruktor initialisiert die Klasse mit benutzerdefinierten seriellen Schnittstellen
 * für AT-Befehle und Debugging, indem er den Konstruktor der Basisklasse _BG96_Serial aufruft.
 *
 * @param atserial Referenz auf die serielle Schnittstelle für AT-Befehle.
 * @param dserial Referenz auf die serielle Schnittstelle für Debugging.
 */
_BG96_Common::_BG96_Common(Stream &atserial, Stream &dserial) : _BG96_Serial(atserial, dserial)
{
}

/**
 * @brief Schaltet das BG96-Modul ein.
 *
 * Diese Methode ruft die InitModule-Methode auf, um das Modul zu initialisieren und einzuschalten.
 *
 * @return true, wenn das Modul erfolgreich eingeschaltet wurde, sonst false.
 */
bool _BG96_Common::TurnOnModule()
{
    digitalWrite(POWKEY_PIN, LOW); // Powkey-Pin auf LOW setzen
    delay(2000);
    digitalWrite(POWKEY_PIN, HIGH);
    delay(2000);
    digitalWrite(POWKEY_PIN, LOW); // Powkey-Pin auf HIGH setzen
    return true;
}

/**
 * @brief Initialisiert das BG96-Modul.
 *
 * Diese Methode konfiguriert die notwendigen Pins, um das Modul einzuschalten und zurückzusetzen.
 * Sie setzt den ENABLE_PWR-Pin auf HIGH, um das Modul einzuschalten, aktiviert dann den RESET_PIN
 * und den POWKEY_PIN, um das Modul zurückzusetzen.
 *
 * @return true, wenn die Initialisierung erfolgreich war, sonst false.
 */
bool _BG96_Common::InitModule()
{
    pinMode(ENABLE_PWR, OUTPUT);
    pinMode(RESET_PIN, OUTPUT);
    digitalWrite(RESET_PIN, LOW);
    pinMode(POWKEY_PIN, OUTPUT);
    PowerOnModule();
    TurnOnModule();
    // Serial.println("Initialized");
    // ResetModule();
    while (readResponseAndSearchChr(RESPONSE_READY[0], 3) != SUCCESS_RESPONSE)
        ;

    return true;
}

bool _BG96_Common::PowerOffModule()
{
    digitalWrite(ENABLE_PWR, LOW);
    digitalWrite(POWKEY_PIN, LOW);

    return true;
}
bool _BG96_Common::PowerOnModule()
{
    digitalWrite(ENABLE_PWR, HIGH);
    return true;
}

/**
 * @brief Setzt das BG96-Modul zurück.
 *
 * Diese Methode setzt den POWKEY_PIN auf HIGH, wartet eine halbe Sekunde und setzt ihn dann wieder auf LOW,
 * um das Modul zurückzusetzen.
 *
 * @return true, wenn das Zurücksetzen erfolgreich war, sonst false.
 */
bool _BG96_Common::ResetModule()
{
    return PowerOffModule() && InitModule();
}

/**
 * @brief Setzt das Ausgabeformat des Moduls.
 *
 * Diese Methode sendet den AT-Befehl zum Setzen des Ausgabeformats an das Modul.
 *
 * @param format true, um das Ausgabeformat zu setzen, sonst false.
 * @return true, wenn der Befehl erfolgreich gesendet und bestätigt wurde, sonst false.
 */
bool _BG96_Common::SetDevOutputformat(bool format)
{
    char cmd[16];
    if (format == true)
    {
        strcpy(cmd, DEV_OUTPUTFORMAT);
        if (sendAndSearch(cmd, RESPONSE_OK, 2))
        {
            return true;
        }
        return false;
    }
    return false;
}

/**
 * @brief Setzt das Befehlsecho des Moduls.
 *
 * Diese Methode aktiviert oder deaktiviert das Echo von AT-Befehlen durch das Modul.
 *
 * @param echo true, um das Echo einzuschalten, false, um es auszuschalten.
 * @return true, wenn der Befehl erfolgreich gesendet und bestätigt wurde, sonst false.
 */
bool _BG96_Common::SetDevCommandEcho(bool echo)
{
    const char *cmd;
    if (echo == true)
    {
        cmd = "E1"; // Echo einschalten
    }
    else
    {
        cmd = "E0"; // Echo ausschalten
    }
    if (sendAndSearch(cmd, RESPONSE_OK, 2))
    {
        return true;
    }
    return false;
}

/**
 * @brief Holt die aktuell synchronisierte GMT-Zeit vom Netzwerk.
 *
 * Diese Methode sendet den entsprechenden AT-Befehl an das Modul und extrahiert die GMT-Zeit aus der Antwort.
 *
 * @param time Ein Puffer, in den die Zeit als Zeichenkette kopiert wird (mindestens 20 Zeichen lang).
 * @return true, wenn die Zeit erfolgreich abgerufen wurde, sonst false.
 */
bool _BG96_Common::GetLatestGMTTime(char *time)
{
    if (sendAndSearch(DEV_GMTTIME, RESPONSE_OK, 2))
    {
        char *start_buf = searchStrBuffer("\"");
        if (start_buf != NULL)
        {
            char *end_buf = strchr(start_buf + 1, '\"');
            if (end_buf != NULL)
            {
                *end_buf = '\0';
                char *timestamp = start_buf + 1;
                strncpy(time, timestamp, 19);
                time[19] = '\0';
                *end_buf = '\"';

                return true;
            }
        }
    }
    return false;
}
char *_BG96_Common::GetCurrentTime()
{

    DevClock(currenttime, READ_MODE);
    return currenttime;
};
/**
 * @brief Holt die Geräteinformationen.
 *
 * Diese Methode sendet den entsprechenden AT-Befehl und kopiert die erhaltenen Geräteinformationen in den bereitgestellten Puffer.
 *
 * @param inf Ein Puffer, in den die Geräteinformationen kopiert werden (mindestens RX_BUFFER_LENGTH Zeichen lang).
 * @return true, wenn die Informationen erfolgreich abgerufen wurden, sonst false.
 */
bool _BG96_Common::GetDevInformation(char *inf)
{
    if (sendAndSearch(DEV_INFORMATION, RESPONSE_OK, 2))
    {
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        strcpy(inf, rxBuffer); // Informationen in inf kopieren
        return true;
    }
    return false;
}

/**
 * @brief Holt die Geräteversion.
 *
 * Diese Methode sendet den entsprechenden AT-Befehl und kopiert die erhaltene Version in den bereitgestellten Puffer.
 *
 * @param ver Ein Puffer, in den die Geräteversion kopiert wird (mindestens RX_BUFFER_LENGTH Zeichen lang).
 * @return true, wenn die Version erfolgreich abgerufen wurde, sonst false.
 */
bool _BG96_Common::GetDevVersion(char *ver)
{
    if (sendAndSearch(DEV_VERSION, RESPONSE_OK, 2))
    {
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        strcpy(ver, rxBuffer);
        return true;
    }
    return false;
}

/**
 * @brief Holt die Geräte-IMEI.
 *
 * Diese Methode sendet den entsprechenden AT-Befehl und kopiert die IMEI in den bereitgestellten Puffer.
 *
 * @param imei Ein Puffer, in den die IMEI kopiert wird (mindestens RX_BUFFER_LENGTH Zeichen lang).
 * @return true, wenn die IMEI erfolgreich abgerufen wurde, sonst false.
 */
bool _BG96_Common::GetDevIMEI(char *imei)
{
    if (sendAndSearch(DEV_IMEI, RESPONSE_OK, 2))
    {
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        strcpy(imei, rxBuffer); // IMEI in imei kopieren
        return true;
    }
    return false;
}

/**
 * @brief Setzt die Funktionalität des Moduls.
 *
 * Diese Methode konfiguriert die Funktionalitätsstufe des Moduls, wie minimale Funktionalität, volle Funktionalität oder das Deaktivieren des RF.
 *
 * @param mode Die gewünschte Funktionalitätsstufe (MINIMUM_FUNCTIONALITY, FULL_FUNCTIONALITY, DISABLE_RF).
 * @return Cmd_Response_t Der Status der Antwort (SUCCESS_RESPONSE, FIAL_RESPONSE, UNKNOWN_RESPONSE, TIMEOUT_RESPONSE).
 */
Cmd_Response_t _BG96_Common::SetDevFunctionality(Functionality_t mode)
{
    char cmd[16];
    Cmd_Response_t fun_status;
    strcpy(cmd, DEV_FUN_LEVEL);
    switch (mode)
    {
    case MINIMUM_FUNCTIONALITY:
        strcat(cmd, "=0"); // Turn off
        break;
    case FULL_FUNCTIONALITY:
        strcat(cmd, "=1"); // Volle Funktionalität
        break;
    case RESET_FUNCTIONALITY:
        strcat(cmd, "=1,1");
        break;
    case DISABLE_RF:
        strcat(cmd, "=4"); // RF deaktivieren
        break;
    default:
        return UNKNOWN_RESPONSE;
    }
    fun_status = sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 15);
    return fun_status;
}

/**
 * @brief Setzt oder liest die lokale Datenrate des Moduls.
 *
 * Diese Methode kann entweder die aktuelle Datenrate auslesen oder eine neue Datenrate einstellen, abhängig vom übergebenen Status.
 *
 * @param rate Referenz auf die Variable, die die Datenrate hält. Beim Lesen wird die gelesene Rate hier gespeichert.
 * @param status Der Modus der Operation (READ_MODE zum Lesen, WRITE_MODE zum Schreiben).
 * @return true, wenn die Operation erfolgreich war, sonst false.
 */
bool _BG96_Common::DevLocalRate(unsigned long &rate, Cmd_Status_t status)
{
    char cmd[16];
    strcpy(cmd, DEV_LOCAL_RATE);
    if (status == READ_MODE)
    {
        strcat(cmd, "?");
        if (sendAndSearch(cmd, RESPONSE_OK, 2))
        {
            char *sta_buf = searchStrBuffer(": ");
            char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
            *end_buf = '\0';
            rate = atol(sta_buf + 2); // Datenrate auslesen
            return true;
        }
    }
    else if (status == WRITE_MODE)
    {
        for (unsigned int i = 0; i < sizeof(Band_list) / sizeof(Band_list[0]); i++)
        {
            if (rate == Band_list[i])
            {
                char buf[16];
                sprintf(buf, "=%ld;&W", rate);
                strcat(cmd, buf);
                if (sendAndSearch(cmd, RESPONSE_OK, 2))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * @brief Holt die SIM-IMSI des Moduls.
 *
 * Diese Methode sendet den entsprechenden AT-Befehl und kopiert die IMSI in den bereitgestellten Puffer.
 *
 * @param imsi Ein Puffer, in den die IMSI kopiert wird (mindestens RX_BUFFER_LENGTH Zeichen lang).
 * @return true, wenn die IMSI erfolgreich abgerufen wurde, sonst false.
 */
bool _BG96_Common::GetDevSimIMSI(char *imsi)
{
    if (sendAndSearch(DEV_SIM_IMSI, RESPONSE_OK, 2))
    {
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        strcpy(imsi, rxBuffer); // IMSI in imsi kopieren
        return true;
    }
    return false;
}

/**
 * @brief Setzt die SIM-PIN des Moduls.
 *
 * Diese Methode kann entweder die aktuelle SIM-PIN lesen oder eine neue SIM-PIN setzen, abhängig vom übergebenen Status.
 *
 * @param pin Der neue PIN-Code als Zeichenkette.
 * @param status Der Modus der Operation (READ_MODE zum Lesen, WRITE_MODE zum Schreiben).
 * @return true, wenn die Operation erfolgreich war, sonst false.
 */
bool _BG96_Common::DevSimPIN(const char *pin, Cmd_Status_t status)
{
    char cmd[16];
    strcpy(cmd, DEV_SIM_PIN);

    if (status == READ_MODE)
    {
        strcat(cmd, "?");
        if (sendAndSearch(cmd, "READY", 2))
        {
            // pin = "READY"; // Diese Zeile ist nicht notwendig, da pin nicht geändert wird
            return true;
        }
    }
    else if (status == WRITE_MODE)
    {
        char buf[16];
        sprintf(buf, "=\"%s\"", pin);
        strcat(cmd, buf);
        if (sendAndSearch(cmd, RESPONSE_OK, 2))
        {
            return true;
        }
    }

    return false;
}

/**
 * @brief Holt die SIM-ICCID des Moduls.
 *
 * Diese Methode sendet den entsprechenden AT-Befehl und kopiert die ICCID in den bereitgestellten Puffer.
 *
 * @param iccid Ein Puffer, in den die ICCID kopiert wird (mindestens RX_BUFFER_LENGTH Zeichen lang).
 * @return true, wenn die ICCID erfolgreich abgerufen wurde, sonst false.
 */
bool _BG96_Common::GetDevSimICCID(char *iccid)
{
    if (sendAndSearch(DEV_SIM_ICCID, RESPONSE_OK, 2))
    {
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        char *sta_buf = searchStrBuffer(": ");
        strcpy(iccid, sta_buf + 2); // ICCID in iccid kopieren
        return true;
    }
    return false;
}

/**
 * @brief Holt den Netzregistrierungsstatus des Moduls.
 *
 * Diese Methode überprüft, ob das Modul im Netzwerk registriert ist, und gibt den entsprechenden Status zurück.
 *
 * @return Net_Status_t Der Netzregistrierungsstatus (REGISTERED, REGISTERED_ROAMING, NOT_REGISTERED).
 */
Net_Status_t _BG96_Common::DevNetRegistrationStatus()
{
    char cmd[16];
    Net_Status_t n_status = NOT_REGISTERED;
    strcpy(cmd, DEV_NET_STATUS_G);
    strcat(cmd, "?");
    if (sendAndSearch(cmd, RESPONSE_OK, 2))
    {
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        char *sta_buf = searchChrBuffer(',');
        n_status = (Net_Status_t)atoi(sta_buf + 1);
        switch (n_status)
        {
        case REGISTERED:
        case REGISTERED_ROAMING:
            return n_status;
        default:
            break;
        }
    }

    strcpy(cmd, DEV_EPS_NET_STATUS);
    strcat(cmd, "?");
    if (sendAndSearch(cmd, RESPONSE_OK, 2))
    {
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        char *sta_buf = searchChrBuffer(',');
        n_status = (Net_Status_t)atoi(sta_buf + 1);
        switch (n_status)
        {
        case REGISTERED:
        case REGISTERED_ROAMING:
            return n_status;
        default:
            break;
        }
    }
    return n_status;
}

/**
 * @brief Holt die Netzsignalqualität des Moduls.
 *
 * Diese Methode sendet den entsprechenden AT-Befehl und extrahiert den RSSI-Wert aus der Antwort.
 *
 * @param rssi Referenz auf die Variable, in die der RSSI-Wert gespeichert wird.
 * @return true, wenn die Signalqualität erfolgreich abgerufen wurde, sonst false.
 */
bool _BG96_Common::GetDevNetSignalQuality(unsigned int &rssi)
{
    if (sendAndSearch(DEV_NET_RSSI, RESPONSE_OK, 2))
    {
        char *sta_buf = searchStrBuffer(": ");
        char *end_buf = searchChrBuffer(',');
        *end_buf = '\0';
        rssi = atoi(sta_buf + 2); // RSSI-Wert auslesen
        return true;
    }
    return false;
}

/**
 * @brief Scannt das Operator-Netzwerk.
 *
 * Diese Methode sendet den entsprechenden AT-Befehl, um verfügbare Netzbetreiber zu scannen, und kopiert den gefundenen Netzbetreiber in den bereitgestellten Puffer.
 *
 * @param net Ein Puffer, in den der Netzbetreiber kopiert wird (mindestens RX_BUFFER_LENGTH Zeichen lang).
 * @return Cmd_Response_t Der Status der Antwort (SUCCESS_RESPONSE, FIAL_RESPONSE, UNKNOWN_RESPONSE, TIMEOUT_RESPONSE).
 */
Cmd_Response_t _BG96_Common::ScanOperatorNetwork(char *net)
{
    char cmd[16];
    Cmd_Response_t scan_status;
    strcpy(cmd, DEV_NET_OPERATOR);
    strcat(cmd, "=?");
    scan_status = sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 180);
    if (scan_status == SUCCESS_RESPONSE)
    {
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        char *sta_buf = searchStrBuffer(": ");
        strcpy(net, sta_buf + 2); // Netzbetreiber in net kopieren
    }
    else if (scan_status == FIAL_RESPONSE)
    {
        char *sta_buf = searchStrBuffer(": ");
        strcpy(net, sta_buf + 2); // Netzbetreiber in net kopieren
    }
    return scan_status;
}

/**
 * @brief Konfiguriert das Operator-Netzwerk.
 *
 * Diese Methode kann entweder das aktuelle Operator-Netzwerk auslesen oder ein neues Netzwerk konfigurieren,
 * abhängig vom übergebenen Status.
 *
 * @param mode Referenz auf die Variable, die den Netzwerkmodus hält.
 * @param format Referenz auf die Variable, die das Netzwerkformat hält.
 * @param oper Ein Puffer, in den der Operator-Name kopiert wird.
 * @param act Referenz auf die Variable, die den aktiven Netztyp hält.
 * @param status Der Modus der Operation (READ_MODE zum Lesen, WRITE_MODE zum Schreiben).
 * @return Cmd_Response_t Der Status der Antwort (SUCCESS_RESPONSE, FIAL_RESPONSE, UNKNOWN_RESPONSE, TIMEOUT_RESPONSE).
 */
Cmd_Response_t _BG96_Common::DevOperatorNetwork(unsigned int &mode, unsigned int &format, const char *oper, Net_Type_t &act, Cmd_Status_t status)
{
    char cmd[16];
    Cmd_Response_t oper_status = UNKNOWN_RESPONSE;
    strcpy(cmd, DEV_NET_OPERATOR);
    if (status == READ_MODE)
    {
        strcat(cmd, "?");
        oper_status = sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5);
        if (oper_status == SUCCESS_RESPONSE)
        {
            char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
            *end_buf = '\0';
            char *sta_buf = searchStrBuffer(": ");
            char message[64];
            char *p[5];
            int i = 0;
            strcpy(message, sta_buf + 2);
            p[0] = strtok(message, ",");
            while (p[i] != NULL)
            {
                i++;
                p[i] = strtok(NULL, ",");
            }
            // Sicherstellen, dass die Zeichenkette nullterminiert ist
            if (i < 5)
            {
                p[i] = NULL;
            }
            mode = atoi(p[0]);
            format = atoi(p[1]);

            // Lokalen Puffer für oper verwenden
            // static char local_oper[32];
            // strncpy(local_oper, p[2], sizeof(local_oper) - 1);
            // local_oper[sizeof(local_oper) - 1] = '\0';
            // oper = local_oper; // oper zeigt jetzt auf den lokalen Puffer

            act = (Net_Type_t)atoi(p[3]);
        }
    }
    else if (status == WRITE_MODE)
    {
        char buf[32];
        if (mode != 0)
        {
            sprintf(buf, "=%d,%d,\"%s\",%d", mode, format, oper, act);
        }
        else
            sprintf(buf, "=%d", mode);
        strcat(cmd, buf);
        oper_status = sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 30);
    }
    return oper_status;
}
/**
 * @brief Holt die Netzwerkinformationen des Moduls.
 *
 * Diese Methode sendet den entsprechenden AT-Befehl und extrahiert Informationen wie Typ, Operator, Band und Kanal aus der Antwort.
 *
 * @param type Ein Puffer, in den der Netztyp kopiert wird (mindestens RX_BUFFER_LENGTH Zeichen lang).
 * @param oper Ein Puffer, in den der Operator-Name kopiert wird (mindestens RX_BUFFER_LENGTH Zeichen lang).
 * @param band Ein Puffer, in den das Netzband kopiert wird (mindestens RX_BUFFER_LENGTH Zeichen lang).
 * @param channel Ein Puffer, in den der Kanal kopiert wird (mindestens RX_BUFFER_LENGTH Zeichen lang).
 * @return true, wenn die Netzwerkinformationen erfolgreich abgerufen wurden, sonst false.
 */
bool _BG96_Common::GetDevNetworkInformation(char *type, char *oper, char *band, char *channel)
{
    if (sendAndSearch(DEV_NET_INFORMATION, RESPONSE_OK, 2))
    {
        char *sta_buf = searchStrBuffer(": ");
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        char message[64];
        char *p[5];
        int i = 0;
        strcpy(message, sta_buf + 2);
        p[0] = strtok(message, ",");
        while (p[i] != NULL)
        {
            i++;
            p[i] = strtok(NULL, ",");
        }
        if (p[i] == NULL)
        {
            message[strlen(message)] = '\0';
        }
        strcpy(type, p[0]);
        strcpy(oper, p[1]);
        strcpy(band, p[2]);
        strcpy(channel, p[3]);
        return true;
    }
    return false;
}

/**
 * @brief Holt den Netzpaketzähler des Moduls.
 *
 * Diese Methode sendet den entsprechenden AT-Befehl, um die Anzahl der gesendeten und empfangenen Bytes abzurufen.
 * Optional kann der Zähler nach dem Abrufen zurückgesetzt werden.
 *
 * @param send_bytes Referenz auf die Variable, in die die Anzahl der gesendeten Bytes kopiert wird.
 * @param recv_bytes Referenz auf die Variable, in die die Anzahl der empfangenen Bytes kopiert wird.
 * @param clean true, um den Zähler nach dem Abrufen zurückzusetzen, sonst false.
 * @return true, wenn die Operation erfolgreich war, sonst false.
 */
bool _BG96_Common::DevNetPacketCounter(unsigned long &send_bytes, unsigned long &recv_bytes, bool clean)
{
    char cmd[16];
    strcpy(cmd, DEV_NET_PACKET_COUNTER);
    strcat(cmd, "?");
    if (sendAndSearch(cmd, RESPONSE_OK, 2))
    {
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        char *med_buf = searchChrBuffer(',');
        *med_buf = '\0';
        char *sta_buf = searchStrBuffer(": ");
        send_bytes = atol(sta_buf + 2);
        recv_bytes = atol(med_buf + 1);
        if (clean == true)
        {
            memset(cmd, '\0', 16);
            strcpy(cmd, DEV_NET_PACKET_COUNTER);
            strcat(cmd, "=0");
            if (sendAndSearch(cmd, RESPONSE_OK, 2))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

/**
 * @brief Schaltet das Modul herunter.
 *
 * Diese Methode sendet den entsprechenden AT-Befehl, um das Modul in den Power-Down-Modus zu versetzen.
 *
 * @return true, wenn der Befehl erfolgreich gesendet und bestätigt wurde, sonst false.
 */
bool _BG96_Common::DevPowerDown()
{
    char cmd[16];
    strcpy(cmd, DEV_POWER_DOWN);
    strcat(cmd, "=1");
    if (sendAndSearch(cmd, RESPONSE_POWER_DOWN, 2))
    {
        return true;
    }
    return false;
}

/**
 * @brief Holt oder setzt die Uhr des Moduls.
 *
 * Diese Methode kann entweder die aktuelle Uhrzeit auslesen oder eine neue Uhrzeit einstellen, abhängig vom übergebenen Status.
 *
 * @param d_clock Ein Puffer, in den die Uhrzeit kopiert wird oder aus dem die neue Uhrzeit gelesen wird (mindestens RX_BUFFER_LENGTH Zeichen lang).
 * @param status Der Modus der Operation (READ_MODE zum Lesen, WRITE_MODE zum Schreiben).
 * @return true, wenn die Operation erfolgreich war, sonst false.
 */
bool _BG96_Common::DevClock(char *d_clock, Cmd_Status_t status)
{
    char cmd[32];
    strcpy(cmd, DEV_CLOCK);
    if (status == READ_MODE)
    {
        strcat(cmd, "?");
        if (sendAndSearch(cmd, RESPONSE_OK, 2))
        {
            char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
            *end_buf = '\0';
            char *sta_buf = searchStrBuffer(": ");
            strcpy(d_clock, sta_buf + 2);
            if (d_clock[0] == '\"')
            {
                memmove(d_clock, d_clock + 1, strlen(d_clock));
                char *quote_pos = strchr(d_clock, '\"');
                if (quote_pos)
                {
                    *quote_pos = '\0';
                }
            }

            return true;
        }
    }
    else if (status == WRITE_MODE)
    {
        char buf[32];
        sprintf(buf, "=\"%s\"", d_clock);
        strcat(cmd, buf);
        if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2))
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Konfiguriert den Scanmodus des Moduls.
 *
 * Diese Methode sendet den entsprechenden AT-Befehl, um den Scanmodus (automatisch, nur GSM, nur LTE) des Moduls einzustellen.
 *
 * @param mode Der gewünschte Scanmodus (0 für automatisch, 1 für nur GSM, 3 für nur LTE).
 * @return true, wenn der Befehl erfolgreich gesendet und bestätigt wurde, sonst false.
 */
bool _BG96_Common::ScanmodeConfig(int mode)
{
    char cmd[32];
    sprintf(cmd, "+QCFG=\"nwscanmode\",%d", mode);

    if (sendAndSearch(cmd, RESPONSE_OK, 2))
    {
        return true;
    }
    return false;
}
bool _BG96_Common::SearchingConfig(const char *scanseq)
{
    char cmd[32];
    sprintf(cmd, "+QCFG=\"nwscanseq\",%s", scanseq);

    if (sendAndSearch(cmd, RESPONSE_OK, 2))
    {
        return true;
    }
    return false;
}
/**
 * @brief Configure Network Category to be Searched under LTE RAT .
 *
 *
 * @param mode Der gewünschte Scanmodus (0 für LTE Cat M1, 1 für LTE Cat NB1, 2 LTE Cat M1 and Cat NB1).
 * @return true, wenn der Befehl erfolgreich gesendet und bestätigt wurde, sonst false.
 */
bool _BG96_Common::LTENetworkCategoryConfig(int mode)
{
    char cmd[32];
    sprintf(cmd, "+QCFG=\"iotopmode\",%d", mode);

    if (sendAndSearch(cmd, RESPONSE_OK, 2))
    {
        return true;
    }
    return false;
}
/**
 * @brief Configure Network Category to be Searched under LTE RAT .
 *
 *
 * @param service Der gewünschte Service (1 für PS bzw. nur Datenumtausch, 2 für CS&PS auch Anrufe).
 * @return true, wenn der Befehl erfolgreich gesendet und bestätigt wurde, sonst false.
 */
bool _BG96_Common::ServiceDomainConfig(int service)
{
    char cmd[32];
    sprintf(cmd, "+QCFG=\"servicedomain\",%d", service);

    if (sendAndSearch(cmd, RESPONSE_OK, 2))
    {
        return true;
    }
    return false;
}
/**
 * @brief Konfiguriert die Frequenzbänder des BG96-Moduls für GSM, CAT-M1 und CAT-NB1.
 *
 * Diese Methode formatiert und sendet einen AT-Befehl an das BG96-Modul, um die zu verwendenden Frequenzbänder für GSM, CAT-M1 und CAT-NB1 festzulegen.
 * Die Methode akzeptiert drei String-Parameter, die die spezifischen Bandwerte für GSM, CAT-M1 und CAT-NB1 angeben und
 * sendet den Befehl an das Modul. Falls der Befehl erfolgreich ist, wird `true` zurückgegeben, andernfalls `false`.
 *
 * @param gsmbandval Ein String, der das Band für GSM spezifiziert.
 * @param catm1bandval Ein String, der das Band für LTE CAT-M1 spezifiziert.
 * @param catnb1bandval Ein String, der das Band für LTE CAT-NB1 spezifiziert.
 *
 * @return `true`, wenn die Konfiguration erfolgreich war (d.h. eine positive Antwort auf den gesendeten Befehl erhalten wurde),
 *         `false` bei einem Fehler.
 *
 * @note Die Methode verwendet den Befehl `+QCFG="band"` des BG96-Moduls und erwartet als Antwort `RESPONSE_OK`.
 *       Die Wartezeit auf die Antwort beträgt 2 Sekunden.
 */
bool _BG96_Common::BandConfig(const char *gsmbandval, const char *catm1bandval, const char *catnb1bandval)
{
    char cmd[32];
    sprintf(cmd, "+QCFG=\"band\",%s,%s,%s", gsmbandval, catm1bandval, catnb1bandval);

    if (sendAndSearch(cmd, RESPONSE_OK, 2))
    {
        return true;
    }
    return false;
}
/**
 * @brief Wandelt einen Zeitstempel im Format "YYYY/MM/DD,HH:MM:SS" in Unix-Zeit (time_t) um.
 *
 * @param timestamp Der Zeitstempel als Zeichenkette im Format "YYYY/MM/DD,HH:MM:SS".
 * @return time_t Die entsprechende Unix-Zeit.
 */
time_t _BG96_Common::parseTimestamp(const char *timestamp)
{
    int year, month, day, hour, minute, second;

    // Der Zeitstempel wird zerlegt in Jahr, Monat, Tag, Stunde, Minute, Sekunde
    sscanf(timestamp, "%d/%d/%d,%d:%d:%d", &year, &month, &day, &hour, &minute, &second);

    struct tm t = {};
    t.tm_year = year - 1900; // tm_year zählt ab 1900
    t.tm_mon = month - 1;    // tm_mon ist 0-basiert
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min = minute;
    t.tm_sec = second;

    // Konvertiert den Zeitstempel in Unix-Zeit (Sekunden seit 1. Januar 1970)
    return mktime(&t);
}

// Adjusted method signature
Cell *_BG96_Common::ReportCellInformation(const char *celltype)
{
    // Prepare and send the command
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "%s=\"%s\"", QUECCELL_ENGINEERING_MODE, celltype);

    if (sendAndSearch(cmd, RESPONSE_OK, 2))
    {
        // Parse the response
        char *start_buf = strstr(rxBuffer, "+QENG: ");
        if (start_buf != nullptr)
        {
            start_buf += strlen("+QENG: ");
            char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
            *end_buf = '\0';
            char infos[512]; // Adjust size if needed
            strcpy(infos, start_buf);

            // Initialize variables
            char cellType[16], state[16], rat[16], duplex_mode[16];
            char *rest = infos;

            // Parse cellType
            char *token = strtok_r(rest, ",", &rest);
            if (token == NULL)
                return nullptr;
            sscanf(token, "\"%[^\"]\"", cellType);

            if (strcmp(cellType, "servingcell") == 0)
            {
                // Parse state
                token = strtok_r(NULL, ",", &rest);
                if (token == NULL)
                    return nullptr;
                sscanf(token, "\"%[^\"]\"", state);

                // Parse rat
                token = strtok_r(NULL, ",", &rest);
                if (token == NULL)
                    return nullptr;
                sscanf(token, "\"%[^\"]\"", rat);

                // Convert rat to lowercase
                for (char *p = rat; *p; ++p)
                    *p = tolower(*p);

                // Parse duplex_mode
                token = strtok_r(NULL, ",", &rest);
                if (token == NULL)
                    return nullptr;
                sscanf(token, "\"%[^\"]\"", duplex_mode);

                if (strcmp(rat, "gsm") == 0)
                {
                    // Ignore GSM servingcell as per your requirements
                    return nullptr;
                }
                else if (strcmp(rat, "cat-m") == 0 || strcmp(rat, "nb-iot") == 0)
                {
                    // Parse mcc
                    token = strtok_r(NULL, ",", &rest);
                    if (token == NULL)
                        return nullptr;
                    int mcc = atoi(token);

                    // Parse mnc
                    token = strtok_r(NULL, ",", &rest);
                    if (token == NULL)
                        return nullptr;
                    int mnc = atoi(token);

                    // Parse cellid (hex)
                    token = strtok_r(NULL, ",", &rest);
                    if (token == NULL)
                        return nullptr;
                    long cellid = strtol(token, NULL, 16); // Hexadecimal

                    // Skip pcid, earfcn, freq_band_ind, ul_bandwidth, dl_bandwidth
                    for (int i = 0; i < 5; i++)
                    {
                        token = strtok_r(NULL, ",", &rest);
                        if (token == NULL)
                            return nullptr;
                    }

                    // Parse tac (hex)
                    token = strtok_r(NULL, ",", &rest);
                    if (token == NULL)
                        return nullptr;
                    int tac = (int)strtol(token, NULL, 16); // Hexadecimal

                    // Parse rsrp
                    token = strtok_r(NULL, ",", &rest);
                    if (token == NULL)
                        return nullptr;
                    int rsrp = atoi(token);

                    // Parse rsrq
                    token = strtok_r(NULL, ",", &rest);
                    if (token == NULL)
                        return nullptr;
                    // int rsrq = atoi(token);

                    // Parse rssi
                    token = strtok_r(NULL, ",", &rest);
                    if (token == NULL)
                        return nullptr;
                    // int rssi = atoi(token);

                    // Parse sinr
                    token = strtok_r(NULL, ",", &rest);
                    if (token == NULL)
                        return nullptr;
                    // int sinr = atoi(token);

                    // Parse srxlev (may be "-" or a number)
                    token = strtok_r(NULL, ",", &rest);
                    if (token == NULL)
                        return nullptr;
                    // srxlev kann "-" oder eine Zahl sein, wir können es hier ignorieren oder nach Bedarf verwenden

                    // Optional: Parse cqi (falls vorhanden)
                    // token = strtok_r(NULL, ",", &rest);

                    int signal = rsrp; // Verwenden Sie rsrp als Signalstärke

                    // Create and return Cell object
                    return new Cell(rat, mcc, mnc, tac, cellid, signal);
                }
                else
                {
                    // Unsupported RAT
                    return nullptr;
                }
            }
            else
            {
                // Ignore other cell types
                return nullptr;
            }
        }
    }
    return nullptr;
}

int _BG96_Common::ReportNeighbourCellInformation(Cell *cells[], int max_cells)
{
    int cellCount = 0;

    // Prepare and send the command
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "%s=\"neighbourcell\"", QUECCELL_ENGINEERING_MODE);

    if (sendAndSearch(cmd, RESPONSE_OK, 2))
    {
        // Parse the response
        char *start_buf = strstr(rxBuffer, "+QENG: ");
        if (start_buf != nullptr)
        {
            char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
            *end_buf = '\0';
            char infos[2048]; // Adjust size as needed
            strcpy(infos, start_buf);

            char *line = strtok(infos, "\n");

            while (line != NULL && cellCount < max_cells)
            {
                if (strstr(line, "\"neighbourcell\"") != NULL)
                {
                    char *rest_line = line;
                    char cellType[16], rat[16];

                    // Parse cellType
                    char *token = strtok_r(rest_line, ",", &rest_line);
                    sscanf(token, "\"%[^\"]\"", cellType);

                    // Parse rat
                    token = strtok_r(NULL, ",", &rest_line);
                    sscanf(token, "\"%[^\"]\"", rat);

                    // Convert rat to lowercase
                    for (char *p = rat; *p; ++p)
                        *p = tolower(*p);

                    if (strcmp(rat, "gsm") == 0)
                    {
                        // GSM neighbour cell parsing
                        int mcc = atoi(strtok_r(NULL, ",", &rest_line));
                        int mnc = atoi(strtok_r(NULL, ",", &rest_line));
                        int lac = (int)strtol(strtok_r(NULL, ",", &rest_line), NULL, 16);      // Hexadecimal
                        long cellid = (long)strtol(strtok_r(NULL, ",", &rest_line), NULL, 16); // Hexadecimal

                        // Skip bsic, arfcn
                        for (int i = 0; i < 2; i++)
                            strtok_r(NULL, ",", &rest_line);

                        // Get rxlev and calculate signal
                        int rxlev = atoi(strtok_r(NULL, ",", &rest_line));
                        int signal = rxlev;

                        // Create and add Cell object
                        cells[cellCount++] = new Cell("gsm", mcc, mnc, lac, cellid, signal);
                    }
                }

                // Get next line
                line = strtok(NULL, "\n");
            }
        }
    }
    return cellCount;
}

bool _BG96_Common::ResetFunctionality()
{
    if (!SetDevFunctionality(RESET_FUNCTIONALITY))
        return false;
    while (readResponseAndSearch(RESPONSE_READY, 3) != SUCCESS_RESPONSE)
        ;
    delay(300);
    return true;
}
bool _BG96_Common::ConfigNetworks(const char *rat)
{
    SetDevFunctionality(MINIMUM_FUNCTIONALITY);
    LTENetworkCategoryConfig(2);       // LTE Cat M1 and Cat NB1
    ServiceDomainConfig(1);            // Nur Datenumtausch
    BandConfig("F", "80084", "80084"); // LTE-M + NBIoT on B3/B8/B20 only

    if (strcmp(rat, "gsm") == 0)
    {
        SearchingConfig("010203");
        ScanmodeConfig(1);
    }
    else if (strcmp(rat, "nbiot") == 0)
    {
        SearchingConfig("030201");
        ScanmodeConfig(3);
    }
    else
    {
        SearchingConfig("00");
        ScanmodeConfig(0); // LTE-M fallback NBIoT,LTE
    }
    ResetFunctionality();
    return true;
}

bool _BG96_Common::NetworkRegistrationCodeConfig(int n)
{
    char cmd[32];
    sprintf(cmd, "%s=%d", DEV_EPS_NET_STATUS, n);

    if (sendAndSearch(cmd, RESPONSE_OK, 2))
    {
        return true;
    }
    return false;
}

int _BG96_Common::ScanCells(const char *rat, Cell *cells[])
{
    const int max_cells = 6; // Max number of cells to collect
    int cellCount = 0;       // Counter for the number of cells found

    // Determine scan mode based on 'rat'

    if (strcmp(rat, "lte") == 0)
    {
        Net_Type_t act = LTE_CAT_M1; // LTE network type
        // --- LTE Scanning ---
        // List of operators to scan
        // int operators[3] = {26201, 26202, 26203};

        bool telekom = false;
        bool vodafone = false;
        bool o2 = false;

        for (int i = 0; i < 3; i++)
        {
            // Wait for registration with a maximum timeout of 30 seconds
            Net_Status_t i_status = NOT_REGISTERED;
            unsigned long start_time = millis();
            while (i_status != REGISTERED && i_status != REGISTERED_ROAMING && i_status != REGISTRATION_DENIED)
            {
                i_status = DevNetRegistrationStatus();
                if (millis() - start_time >= 30 * 1000UL) // Timeout after 30 seconds
                {
                    break;
                }
                delay(3000); // Wait 3 seconds
            }
            Cell *cell = ReportCellInformation("servingcell");

            if (cell != nullptr)
            {
                // Check for duplicates
                bool isNewCell = true;
                for (int j = 0; j < cellCount; j++)
                {
                    if (cells[j]->getCellID() == cell->getCellID())
                    {
                        isNewCell = false;
                        delete cell; // Avoid memory leak
                        break;
                    }
                }
                if (isNewCell && cellCount < max_cells)
                {
                    cells[cellCount++] = cell;
                }
                int act_operator = cell->getOperator();
                // Serial.println(act_operator);
                if (act_operator == 26201)
                {
                    telekom = true;
                    // Serial.println("Telekom");
                }
                if (act_operator == 26202)
                {
                    vodafone = true;
                    // Serial.println("Vodafone");
                }
                if (act_operator == 26203)
                {
                    o2 = true;
                    // Serial.println("o2");
                }
            }
            // Configure operator network
            unsigned int mode = 1;   // Manual mode
            unsigned int format = 2; // Numeric format
            if (!o2)
            {
                DevOperatorNetwork(mode, format, "26203", act, WRITE_MODE);
                continue;
            }
            if (!vodafone)
            {
                DevOperatorNetwork(mode, format, "26202", act, WRITE_MODE);
                continue;
            }
            if (!telekom)
            {
                DevOperatorNetwork(mode, format, "26201", act, WRITE_MODE);
                continue;
            }
        }
    }
    else if (strcmp(rat, "nbiot") == 0)
    {
        Net_Status_t i_status = NOT_REGISTERED;
        unsigned long start_time = millis();

        // while (i_status != REGISTERED && i_status != REGISTERED_ROAMING)
        // {
        //     i_status = DevNetRegistrationStatus();
        //     if (millis() - start_time >= 30 * 1000UL) // Timeout after 30 seconds
        //     {

        //         break;
        //     }
        //     delay(3000); // Wait 3 seconds
        // }
        if(!checkForNetwork()){
            return 0;
        }
        Cell *cell = ReportCellInformation("servingcell");
        return 1;
    }
    else if (strcmp(rat, "gsm") == 0)
    {
        // --- GSM Scanning ---
        // Wait for registration with a maximum timeout of 30 seconds
        Net_Status_t i_status = NOT_REGISTERED;
        unsigned long start_time = millis();

        // while (i_status != REGISTERED && i_status != REGISTERED_ROAMING)
        // {
        //     i_status = DevNetRegistrationStatus();
        //     if (millis() - start_time >= 30 * 1000UL) // Timeout after 30 seconds
        //     {
        //         return 0;
        //     }
        //     delay(3000); // Wait 3 seconds
        // }
        if(!checkForNetwork()){
            return 0;
        }
        // Get neighbour cell information
        // delay(10000);
        int neighbourCellCount = ReportNeighbourCellInformation(cells, max_cells);

        cellCount = neighbourCellCount > max_cells ? max_cells : neighbourCellCount;
    }

    return cellCount; // Return the total number of cells found
}
bool _BG96_Common::FactoryReset()
{
    char reset_cmd[32];
    sprintf(reset_cmd, "&F");
    if (sendATcommand(reset_cmd))
    {
        return true;
    }
    return false;
}
bool _BG96_Common::SaveSetting()
{
    char save_cmd[32];
    sprintf(save_cmd, "&W");
    if (sendATcommand(save_cmd))
    {
        return true;
    }
    return false;
}
bool _BG96_Common::checkForNetwork()
{
    Net_Status_t i_status = NOT_REGISTERED;
    unsigned long start_time = millis();
    while (i_status != REGISTERED && i_status != REGISTERED_ROAMING)
    {
        i_status = DevNetRegistrationStatus();
        if (millis() - start_time >= 30 * 1000UL) // Timeout nach 90 Sekunden
        {
            // if(ResetModule())
            // Serial.println("Fail to register!!!");
            return false;
        }
        delay(3000);
    }
    return true;
}