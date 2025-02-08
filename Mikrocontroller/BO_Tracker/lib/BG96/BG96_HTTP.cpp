#include "BG96_HTTP.h"

/**
 * @brief Konstruktor der Klasse _BG96_HTTP.
 *
 * Initialisiert das HTTP-Objekt. Es werden keine speziellen Ressourcen angefordert.
 */
_BG96_HTTP::_BG96_HTTP()
{

}

/**
 * @brief Destruktor der Klasse _BG96_HTTP.
 *
 * Bereinigt alle Ressourcen, die während der Lebensdauer des Objekts verwendet wurden.
 */
_BG96_HTTP::~_BG96_HTTP()
{

}

/**
 * @brief Konstruktor der Klasse _BG96_HTTP mit serieller Schnittstelle.
 *
 * Initialisiert das HTTP-Objekt und setzt die Stream-Parameter für die AT-Kommunikation.
 * 
 * @param atserial AT-Befehls-Schnittstelle
 * @param dserial Debug-Schnittstelle
 */
_BG96_HTTP::_BG96_HTTP(Stream &atserial, Stream &dserial) : _BG96_SSL(atserial, dserial)
{

}

/**
 * @brief Konfiguriert die HTTP-Parameter.
 *
 * Diese Methode verwendet den AT-Befehl AT+QHTTPCFG, um HTTP-Konfigurationsparameter zu setzen.
 * Siehe Quectel BG96 HTTP(S) AT Commands Manual, Abschnitt 2.1 (Seite 8)
 *
 * @param pdp_index PDP-Kontextindex (1-16)
 * @param request_header Boolescher Wert, ob die Anfrage-Header aktiviert werden sollen
 * @param response_header Boolescher Wert, ob die Antwort-Header ausgegeben werden sollen
 * @param type Datentyp des HTTP-Bodys (Text, Binär, etc.)
 * @return true bei Erfolg, false bei Fehler
 */
bool _BG96_HTTP::SetHTTPConfigParameters(unsigned int pdp_index, bool request_header, bool response_header, HTTP_Body_Data_Type_t type)
{
    char cmd[64], buf[32];
    strcpy(cmd, HTTP_CONFIG_PARAMETER);
    sprintf(buf, "=\"contextid\",%d", pdp_index);
    strcat(cmd, buf);
    if(!sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        return false;
    }
    memset(cmd, '\0', 64);
    strcpy(cmd, HTTP_CONFIG_PARAMETER);
    if (request_header){
        strcat(cmd, "=\"requestheader\",1");
    }else {
        strcat(cmd, "=\"requestheader\",0");
    }
    if(!sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        return false;
    }
    memset(cmd, '\0', 64);
    strcpy(cmd, HTTP_CONFIG_PARAMETER);
    if (response_header){
        strcat(cmd, "=\"responseheader\",1");
    }else {
        strcat(cmd, "=\"responseheader\",0");
    }
    if(!sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        return false;
    }
    memset(cmd, '\0', 64);
    memset(buf, '\0', 32);
    strcpy(cmd, HTTP_CONFIG_PARAMETER);
    sprintf(buf, "=\"contenttype\",%d", type);
    strcat(cmd, buf);
    if(!sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        return false;
    }
    return true;
}

/**
 * @brief Aktiviert SSL für die HTTP-Verbindung.
 *
 * Diese Methode verwendet den AT-Befehl AT+QHTTPCFG, um SSL zu aktivieren.
 * Siehe Quectel BG96 HTTP(S) AT Commands Manual, Abschnitt 2.1 (Seite 8)
 *
 * @param ssl_index Index des SSL-Kontextes (0-5)
 * @return true bei Erfolg, false bei Fehler
 */
bool _BG96_HTTP::SetHTTPEnableSSL(unsigned int ssl_index)
{
    char cmd[64], buf[32];
    strcpy(cmd, HTTP_CONFIG_PARAMETER);
    sprintf(buf, "=\"sslctxid\",%d", ssl_index);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        return true;
    }
    return false;
}

/**
 * @brief Setzt die URL des HTTP-Servers.
 *
 * Diese Methode verwendet den AT-Befehl AT+QHTTPURL, um die Ziel-URL zu setzen oder abzufragen.
 * Siehe Quectel BG96 HTTP(S) AT Commands Manual, Abschnitt 2.2 (Seite 10)
 *
 * @param url Die URL des HTTP-Servers
 * @param status Modus (Lesen oder Schreiben der URL)
 * @return true bei Erfolg, false bei Fehler
 */
bool _BG96_HTTP::HTTPURL(const char* url, Cmd_Status_t status)
{
    char cmd[32], buf[16];
    strcpy(cmd, HTTP_SET_URL);
    if(status == READ_MODE){
        strcat(cmd, "?");
        if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
            char *sta_buf = searchStrBuffer(": ");
            if(sta_buf == NULL){
                char* modifiable_url = const_cast<char*>(url);
                if(modifiable_url != nullptr && strlen(modifiable_url) > 0) {
                    modifiable_url[0] = '\0';  // Setze den Puffer auf einen leeren String
                }
            } else {
                char *end_buf = searchStrBuffer("\r\nOK");
                if (end_buf != NULL) {
                    *end_buf = '\0';
                    strcpy(const_cast<char*>(url), sta_buf + 2);  // URL aus dem Antwortpuffer kopieren
                } else {
                    char* modifiable_url = const_cast<char*>(url);
                    if(modifiable_url != nullptr && strlen(modifiable_url) > 0) {
                        modifiable_url[0] = '\0';
                    }
                }
            }
            return true;
        }
    } else if (status == WRITE_MODE){
        sprintf(buf, "=%d", strlen(url));
        strcat(cmd, buf);
        if(sendAndSearch(cmd, RESPONSE_CONNECT, RESPONSE_ERROR, 10)){
            if(sendDataAndCheck(url, RESPONSE_OK, RESPONSE_ERROR, 60)){
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief Führt eine HTTP-GET-Anfrage aus.
 *
 * Diese Methode verwendet den AT-Befehl AT+QHTTPGET, um eine HTTP-GET-Anfrage auszuführen.
 * Sie wird verwendet, um eine Datei oder Daten von einem Server herunterzuladen, wie z.B. bei der gpsOneXTRA-Datenaktualisierung.
 *
 * @param timeout Die maximale Zeit (in Sekunden), die der Befehl auf eine Antwort warten soll.
 * @return true bei erfolgreichem Download, false bei einem Fehler.
 *
 * @see Quectel BG96 HTTP(S) AT Commands Manual, Abschnitt 2.4 (Seite 14)
 */

bool _BG96_HTTP::HTTPGET(unsigned int timeout)
{
    char cmd[32], buf[16];
    strcpy(cmd, HTTP_GET_REQUEST);
    sprintf(buf, "=%d", timeout);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, HTTP_GET_REQUEST, RESPONSE_ERROR, timeout)){
        unsigned long start_time = millis();
        errorCode = -1;
        while(millis() - start_time < 200UL){
            if(serialAvailable()){
                readResponseByteToBuffer();
            }
        }
        char *sta_buf = searchStrBuffer(": ");
        char *end_buf = searchChrBuffer(',');
        *end_buf = '\0';
        if (atoi(sta_buf + 2) == 0) {
            return true;
        }else {
            errorCode = atoi(sta_buf + 2);
        }
    }
    return false;
}
/**
 * @brief Führt eine HTTP-GET-Anfrage aus.
 *
 * Diese Methode verwendet den AT-Befehl AT+QHTTPGET, um eine GET-Anfrage an den HTTP-Server zu senden.
 * Siehe Quectel BG96 HTTP(S) AT Commands Manual, Abschnitt 2.3 (Seite 11)
 *
 * @param timeout Timeout in Sekunden für die HTTP-GET-Anfrage
 * @return true bei Erfolg, false bei Fehler
 */
/**
 * @brief Führt eine HTTP-POST-Anfrage mit Daten aus.
 *
 * Diese Methode verwendet den AT-Befehl AT+QHTTPPOST, um eine POST-Anfrage an den HTTP-Server zu senden.
 * Siehe Quectel BG96 HTTP(S) AT Commands Manual, Abschnitt 2.4 (Seite 12)
 *
 * @param post_data Die zu sendenden POST-Daten.
 * @param timeout Timeout in Sekunden für die HTTP-POST-Anfrage.
 * @return true bei Erfolg, false bei Fehler.
 */
// bool _BG96_HTTP::HTTPPOST(char *post_data, unsigned int timeout)
// {
//     char cmd[32], buf[16];
//     strcpy(cmd, HTTP_POST_REQUEST);
//     sprintf(buf, "=%d,%d,%d", strlen(post_data), timeout, timeout);
//     strcat(cmd, buf);
//     if(sendAndSearch(cmd, RESPONSE_CONNECT, RESPONSE_ERROR, 10)){
//         if(sendDataAndCheck(post_data, HTTP_POST_REQUEST, RESPONSE_ERROR, timeout)){
//             unsigned long start_time = millis();
//             errorCode = -1;
//             while(millis() - start_time < 200UL){
//                 if(serialAvailable()){
//                     readResponseByteToBuffer();
//                 }
//             }
//             char *sta_buf = searchStrBuffer(": ");
//             char *end_buf = searchChrBuffer(',');
//             *end_buf = '\0';
//             if (atoi(sta_buf + 2) == 0) {
//                 return true;
//             } else {
//                 errorCode = atoi(sta_buf + 2);
//             }
//         }
//     }
//     return false;
// }
bool _BG96_HTTP::HTTPPOST(char *post_data, unsigned int timeout)
{
    // Beispiel: HTTP_POST_REQUEST könnte "AT+QHTTPPOST" sein,
    // RESPONSE_CONNECT -> z.B. "CONNECT"
    // RESPONSE_ERROR   -> z.B. "ERROR"
    // StaBuffer und EndBuffer-Signaturen können von dir abweichen
    delay(200);
    char cmd[32];
    char buf[16];

    // Kommandostring "AT+QHTTPPOST=payloadLen,timeout,timeout"
    strcpy(cmd, HTTP_POST_REQUEST);            // z.B. "AT+QHTTPPOST"
    sprintf(buf, "=%d,%d,%d", strlen(post_data), timeout, timeout);
    strcat(cmd, buf);

    // 1) Sende Kommando und warte auf CONNECT oder ERROR
    //    Wartezeit hier beispielhaft 10s. Ggf. anpassen.
    if (!sendAndSearch(cmd, RESPONSE_CONNECT, RESPONSE_ERROR, timeout/2)) {
        // Falls weder CONNECT noch ERROR kam, Abbruch.
        // errorCode kann man noch spezifisch setzen (z.B. -1)
        return false;
    }
    // delay(300);
    // 2) Daten (post_data) senden
    //    sendDataAndCheck: Wartet auf Antwort (z.B. "...OK" oder "...ERROR")
    if (!sendDataAndCheck(post_data, HTTP_POST_REQUEST, RESPONSE_ERROR, timeout)) {
        // Senden der Daten war nicht erfolgreich
        return false;
    }

    // 3) Puffer füllen (Lese die Antwort vom BG96), z.B. 500ms statt 200
    unsigned long start_time = millis();
    errorCode = -1;
    while (millis() - start_time < 500UL) {  // 500 ms warten, optional anpassen
        if (serialAvailable()) {
            readResponseByteToBuffer();
        }
    }

    // 4) Suche Start- und End-Pointer:
    //    - sta_buf zeigt auf Zeichen nach ": "
    //    - end_buf zeigt auf ','
    char *sta_buf = searchStrBuffer(": ");     // z.B. "QHTTPPOST: 0, ..."
    if (!sta_buf) {
        // Falls nicht gefunden, Abbruch
        errorCode = -2;
        return false;
    }

    char *end_buf = searchChrBuffer(',');      // Suche erstes Komma
    if (!end_buf) {
        // Falls kein Komma gefunden, Abbruch
        errorCode = -3;
        return false;
    }

    // 5) String an dieser Stelle terminieren
    *end_buf = '\0';

    // 6) Parse das, was hinter ": " steht.
    //    Beispiel: "QHTTPPOST: 0" -> sta_buf zeigt auf ": " in "...: 0"
    //    => sta_buf + 2 zeigt auf '0'
    int postResult = atoi(sta_buf + 2);

    // 7) Auswertung: Wenn 0 -> OK, sonst Fehlercode
    if (postResult == 0) {
        // Erfolg
        return true;
    } else {
        // Fehlercode im BG96-Umfeld
        errorCode = postResult;
        return false;
    }
}


/**
 * @brief Führt eine HTTP-POST-Anfrage aus, bei der eine Datei gesendet wird.
 *
 * Diese Methode verwendet den AT-Befehl AT+QHTTPPOSTFILE, um eine Datei über eine POST-Anfrage zu senden.
 * Siehe Quectel BG96 HTTP(S) AT Commands Manual, Abschnitt 2.5 (Seite 13)
 *
 * @param filename Der Dateiname der zu sendenden Datei.
 * @param timeout Timeout in Sekunden für die Anfrage.
 * @return true bei Erfolg, false bei Fehler.
 */
bool _BG96_HTTP::HTTTPPOSTFile(char *filename, unsigned int timeout)
{
    char cmd[64], buf[32];
    strcpy(cmd, HTTP_FILE_POST_REQUEST);
    sprintf(buf, "=\"%s\",%d", filename, timeout);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, HTTP_FILE_POST_REQUEST, RESPONSE_ERROR, timeout)){
        unsigned long start_time = millis();
        errorCode = -1;
        while(millis() - start_time < 200UL){
            if(serialAvailable()){
                readResponseByteToBuffer();
            }
        }
        char *sta_buf = searchStrBuffer(": ");
        char *end_buf = searchChrBuffer(',');
        *end_buf = '\0';
        if (atoi(sta_buf + 2) == 0) {
            return true;
        } else {
            errorCode = atoi(sta_buf + 2);
        }
    }
    return false;
}

/**
 * @brief Liest die HTTP-Antwort in den Speicher.
 *
 * Diese Methode verwendet den AT-Befehl AT+QHTTPREAD, um die HTTP-Antwort in den angegebenen Speicher zu lesen.
 * Siehe Quectel BG96 HTTP(S) AT Commands Manual, Abschnitt 2.6 (Seite 14)
 *
 * @param read_data Speicher, in den die Antwort gelesen wird.
 * @param timeout Timeout in Sekunden für die Leseanfrage.
 * @return true bei Erfolg, false bei Fehler.
 */
// bool _BG96_HTTP::HTTPRead(char *read_data, unsigned int timeout)
// {
//     char cmd[32], buf[16];
//     strcpy(cmd, HTTP_READ_RESPONSE);
//     sprintf(buf, "=%d", timeout);
//     strcat(cmd, buf);
//     if(sendAndSearch(cmd, HTTP_READ_RESPONSE, RESPONSE_ERROR, timeout)){
//         unsigned long start_time = millis();
//         errorCode = -1;
//         while(millis() - start_time < 200UL){
//             if(serialAvailable()){
//                 readResponseByteToBuffer();
//             }
//         }
//         char *sta_buf = searchStrBuffer(RESPONSE_CONNECT);
//         char *end_buf = searchStrBuffer("OK\r\n\r\n");
//         *end_buf = '\0';
//         strcpy(read_data, sta_buf + strlen(RESPONSE_CONNECT) + 2);
//         return true;
//     }
//     return false;
// }

bool _BG96_HTTP::HTTPRead(char *read_data,unsigned int timeout)
{
    char cmd[32];
    char buf[16];
    unsigned int maxLen = 256;
    // Kommandostring bauen, z.B. "AT+QHTTPREAD=30"
    strcpy(cmd, HTTP_READ_RESPONSE);             // z.B. "AT+QHTTPREAD"
    sprintf(buf, "=%d", timeout);
    strcat(cmd, buf);

    // BG96-Kommando abschicken
    // sendAndSearch(cmd, success, error, wartezeit)
    // Gibt true zurück, wenn es "success" findet (HTTP_READ_RESPONSE),
    // sonst false, wenn "error" oder Zeit abgelaufen.
    if (!sendAndSearch(cmd, HTTP_READ_RESPONSE, RESPONSE_ERROR, timeout)) {
        // Kein Erfolg beim Senden / keine passende Antwort
        return false;
    }

    // Warten, bis BG96 alle Daten gesendet hat
    // -> 200 ms sind teils knapp, je nach Paketgröße und Netzwerk
    unsigned long start_time = millis();
    while (millis() - start_time < 500) { // z.B. 500 ms statt 200
        if (serialAvailable()) {
            readResponseByteToBuffer();
        }
    }

    // Pointer auf BEGIN und ENDE im Puffer suchen
    // z.B. BEGIN = "\r\nCONNECT\r\n" (RESPONSE_CONNECT)
    //     ENDE   = "OK\r\n\r\n"
    char *sta_buf = searchStrBuffer(RESPONSE_CONNECT);
    if (!sta_buf) {
        // Falls nicht gefunden, abbrechen
        errorCode = -2;
        return false;
    }

    char *end_buf = searchStrBuffer("OK\r\n\r\n");
    if (!end_buf) {
        // Falls kein "OK\r\n\r\n" gefunden, abbrechen
        errorCode = -3;
        return false;
    }

    // Das Ende terminieren
    *end_buf = '\0';

    // Beispiel: Inhalt beginnt hinter RESPONSE_CONNECT
    // Oft ist RESPONSE_CONNECT z.B. "\r\nCONNECT\r\n"
    // Dann will man ab dem Zeichen NACH diesem String kopieren
    size_t offset = strlen(RESPONSE_CONNECT);
    // Bei manchen Firmwares ist noch ein \r\n dran, also +2
    offset += 2;

    // Pointer auf den eigentlichen Daten-Beginn
    char *startData = sta_buf + offset;
    if (startData < sta_buf || startData >= end_buf) {
        // Nur zur Sicherheit prüfen, ob wir nicht über das Ende hinausrutschen
        errorCode = -4;
        return false;
    }

    // Kopiere in read_data, aber achte auf maxLen (falls sehr große Daten!)
    // Die maximale Länge soll end_buf - startData nicht überschreiten
    size_t copyLen = end_buf - startData;   // effektive Länge
    if (copyLen >= maxLen) {
        copyLen = maxLen - 1;              // Pufferende reservieren für '\0'
    }

    strncpy(read_data, startData, copyLen);
    read_data[copyLen] = '\0'; // String terminieren

    // Wenn alles gut ging:
    errorCode = 0;
    return true;
}


/**
 * @brief Liest die HTTP-Antwort und speichert sie in einer Datei.
 *
 * Diese Methode verwendet den AT-Befehl AT+QHTTPREADFILE, um die HTTP-Antwort in einer Datei zu speichern.
 * Siehe Quectel BG96 HTTP(S) AT Commands Manual, Abschnitt 2.7 (Seite 15)
 *
 * @param filename Der Name der Datei, in die die Antwort gespeichert werden soll.
 * @param timeout Timeout in Sekunden für die Anfrage.
 * @return true bei Erfolg, false bei Fehler.
 */
bool _BG96_HTTP::HTTPReadToFile(const char *filename, unsigned int timeout)
{
    char cmd[64], buf[32];
    strcpy(cmd, HTTP_FILE_READ_RESPONSE);
    sprintf(buf, "=\"%s\",%d", filename, timeout);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, HTTP_FILE_READ_RESPONSE, RESPONSE_ERROR, timeout)){
        unsigned long start_time = millis();
        errorCode = -1;
        while(millis() - start_time < 200UL){
            if(serialAvailable()){
                readResponseByteToBuffer();
            }
        }
        char *sta_buf = searchStrBuffer(": ");
        if(atoi(sta_buf + 2) == 0){
            return true;
        } else {
            errorCode = atoi(sta_buf + 2);
        }
    }
    return false;
}

