#include "BG96_Serial.h"

/**
 * @brief Standardkonstruktor der Klasse _BG96_Serial.
 *
 * Initialisiert die seriellen Schnittstellen _atserial und _dserial auf die Standard-Serial-Schnittstelle (Serial).
 */
_BG96_Serial::_BG96_Serial() : _atserial(Serial), _dserial(Serial)
{
}

/**
 * @brief Destruktor der Klasse _BG96_Serial.
 *
 * Bereinigt den Empfangspuffer, um verbleibende Daten zu löschen.
 */
_BG96_Serial::~_BG96_Serial()
{
    cleanBuffer();
}

/**
 * @brief Parametrisierter Konstruktor der Klasse _BG96_Serial.
 *
 * Initialisiert benutzerdefinierte serielle Schnittstellen für AT-Befehle (atserial) und Debugging (dserial).
 * Setzt die Timeout-Werte und bereinigt den Empfangspuffer.
 *
 * @param atserial Referenz auf die serielle Schnittstelle für AT-Befehle.
 * @param dserial Referenz auf die serielle Schnittstelle für Debugging.
 */
_BG96_Serial::_BG96_Serial(Stream &atserial, Stream &dserial) : _atserial(atserial), _dserial(dserial)
{
    _atserial.setTimeout(2000);
    _dserial.setTimeout(2000);
    cleanBuffer();
}

/**
 * @brief Leitet Daten zwischen den seriellen Schnittstellen _atserial und _dserial weiter.
 *
 * Ermöglicht eine direkte Kommunikation, indem empfangene Daten von einer Schnittstelle an die andere gesendet werden.
 */
void _BG96_Serial::AT_bypass()
{
    while(_atserial.available()){
        _dserial.write(_atserial.read());
    }
    while(_dserial.available()){
        _atserial.write(_dserial.read());
    }
}

/**
 * @brief Versucht automatisch, die optimale Baudrate für die serielle AT-Schnittstelle zu bestimmen.
 *
 * Beginnt mit 115200 Baud und testet anschließend eine Liste vordefinierter Baudraten,
 * bis eine erfolgreiche Kommunikation bestätigt wird.
 *
 * @tparam T Datentyp der seriellen Schnittstelle.
 * @param _atserial Referenz auf die serielle Schnittstelle, deren Baudrate angepasst werden soll.
 * @return true, wenn eine erfolgreiche Kommunikation hergestellt wurde, sonst false.
 */
template <class T>
bool _BG96_Serial::AutoATSerialBand(T &_atserial)
{
    _atserial.begin(115200);
    if(sendAndSearch(DEV_AT, RESPONSE_OK, 2)){
        return true;
    }

    for(int i = 0; i < sizeof(Band_list)/sizeof(Band_list[0]); i++){
        _atserial.end();
        _atserial.begin(Band_list[i]);
        for (int j = 0; j < 3; j++){
            if(sendAndSearch(DEV_AT, RESPONSE_OK, 2)){
                return true; 
            }
        }
    }
    _atserial.end();
    return false;
}

/**
 * @brief Sendet einen Datenpuffer an das Modem und überprüft die Antwort auf vorgegebene Erfolg- oder Fehlermeldungen.
 *
 * Führt eine Verzögerung ein, löscht den Empfangspuffer und sendet die Daten. Bei aktivierter UART_DEBUG-Funktion werden Debug-Informationen ausgegeben.
 *
 * @param data_buf Zeiger auf den zu sendenden Datenpuffer.
 * @param ok_str Erfolgszeichenkette zur Überprüfung der Antwort.
 * @param err_str Fehlerzeichenkette zur Überprüfung der Antwort.
 * @param timeout Timeout in Sekunden für die Antwortüberprüfung.
 * @return true bei erfolgreicher Überprüfung, sonst false.
 */
bool _BG96_Serial::sendDataAndCheck(const char *data_buf, const char *ok_str, const char *err_str, unsigned int timeout)
{
    delay(300);
    while(_atserial.read()>=0);
    int data_len = strlen(data_buf);
    int send_bytes = _atserial.write(data_buf);
#ifdef UART_DEBUG
    _dserial.print("\r\n");
    _dserial.print(data_buf);
    _dserial.print("\r\n");
    _dserial.print("Send Data len :");
    _dserial.print(send_bytes);
    _dserial.print("\r\n");
#endif
    if(send_bytes == data_len){
        if(readResponseAndSearch(ok_str, err_str, timeout)){
            return true;
        }
    }
    return false;
}

/**
 * @brief Sendet einen AT-Befehl an das Modem.
 *
 * Der Befehl wird mit dem Präfix „AT“ und einem abschließenden Zeilenumbruch gesendet. Bei aktivierter UART_DEBUG-Funktion werden Debug-Informationen ausgegeben.
 *
 * @param command Zeiger auf den AT-Befehl, der gesendet werden soll.
 * @return true, wenn der Befehl vollständig gesendet wurde, sonst false.
 */
bool _BG96_Serial::sendATcommand(const char *command)
{
    delay(300);
    while(_atserial.read()>=0);
    _atserial.write("AT");
    int cmd_len = strlen(command);
    int send_bytes = _atserial.write(command);
#if defined UART_DEBUG
    _dserial.print("\r\n");
    _dserial.print("-> ");
    _dserial.print("AT");
    _dserial.print(command);
    _dserial.print("\r\n");
#endif
    if (send_bytes != cmd_len){
        return false;
    }
    _atserial.write("\r\n");
    return true;
}

/**
 * @brief Liest ein Byte von der seriellen AT-Schnittstelle und speichert es im Empfangspuffer.
 *
 * Gibt die Anzahl der gelesenen Bytes zurück.
 *
 * @return Anzahl der gelesenen Bytes (immer 1).
 */
unsigned int _BG96_Serial::readResponseByteToBuffer()
{
    char c = _atserial.read();
    rxBuffer[bufferHead] = c;
    bufferHead = (bufferHead + 1) % RX_BUFFER_LENGTH;
#if defined UART_DEBUG
    if (c == '\n'){
        _dserial.print(c);
        _dserial.print("<- ");
    } else {
        _dserial.print(c);
    }
#endif
    return 1;
}




/**
 * @brief Liest die Antwort des Modems innerhalb eines bestimmten Timeouts und speichert sie im Empfangspuffer.
 *
 * Gibt die Anzahl der empfangenen Bytes zurück.
 *
 * @param timeout Timeout in Sekunden für das Lesen der Antwort.
 * @return Anzahl der empfangenen Bytes.
 */
unsigned int _BG96_Serial::readResponseToBuffer(unsigned int timeout)
{
    unsigned long start_time = millis();
    unsigned int recv_len = 0;
    cleanBuffer();
    while(millis() - start_time < timeout * 1000UL){
        if (serialAvailable() && (bufferHead < RX_BUFFER_LENGTH)) {
            recv_len += readResponseByteToBuffer();
        }
    }
    return recv_len;
}

/**
 * @brief Liest die Antwort des Modems und sucht nach einem bestimmten Zeichen innerhalb eines Timeouts.
 *
 * Gibt den Status der Antwort zurück.
 *
 * @param test_chr Das zu suchende Zeichen.
 * @param timeout Timeout in Sekunden für die Antwortüberprüfung.
 * @return Status der Antwort (SUCCESS_RESPONSE, UNKNOWN_RESPONSE, TIMEOUT_RESPONSE).
 */
Cmd_Response_t _BG96_Serial::readResponseAndSearchChr(char test_chr, unsigned int timeout)
{
    unsigned long start_time = millis();
    unsigned int recv_len = 0;
    cleanBuffer();
    while(millis() - start_time < timeout * 1000UL){
        if (serialAvailable() && (bufferHead < RX_BUFFER_LENGTH)) {
            recv_len += readResponseByteToBuffer();
            if (searchChrBuffer(test_chr)){
                return SUCCESS_RESPONSE;
            }
        } 
    }
    if (recv_len > 0){
        return UNKNOWN_RESPONSE;
    } else {
        return TIMEOUT_RESPONSE;
    }
}

/**
 * @brief Liest die Antwort des Modems und sucht nach einer bestimmten Zeichenkette innerhalb eines Timeouts.
 *
 * Gibt den Status der Antwort zurück.
 *
 * @param test_str Die zu suchende Zeichenkette.
 * @param timeout Timeout in Sekunden für die Antwortüberprüfung.
 * @return Status der Antwort (SUCCESS_RESPONSE, UNKNOWN_RESPONSE, TIMEOUT_RESPONSE).
 */
Cmd_Response_t _BG96_Serial::readResponseAndSearch(const char *test_str, unsigned int timeout)
{
    unsigned long start_time = millis();
    unsigned int recv_len = 0;
    cleanBuffer();
    while(millis() - start_time < timeout * 1000UL){
        if (serialAvailable()){
            recv_len += readResponseByteToBuffer();
            if (searchStrBuffer(test_str)){
                return SUCCESS_RESPONSE;
            }
        } 
    }
    if (recv_len > 0){
        return UNKNOWN_RESPONSE;
    } else {
        return TIMEOUT_RESPONSE;
    }
}

/**
 * @brief Liest die Antwort des Modems und sucht nach einer Erfolgskette oder einer Fehlerkette innerhalb eines Timeouts.
 *
 * Bei einem Fehler werden der Fehlercode extrahiert und zurückgegeben.
 *
 * @param test_str Die zu suchende Erfolgskette.
 * @param e_test_str Die zu suchende Fehlerkette.
 * @param timeout Timeout in Sekunden für die Antwortüberprüfung.
 * @return Status der Antwort (SUCCESS_RESPONSE, FIAL_RESPONSE, UNKNOWN_RESPONSE, TIMEOUT_RESPONSE).
 */
Cmd_Response_t _BG96_Serial::readResponseAndSearch(const char *test_str, const char *e_test_str, unsigned int timeout)
{
    unsigned long start_time = millis();
    unsigned int recv_len = 0;
    errorCode = -1;
    cleanBuffer();
    while(millis() - start_time < timeout * 1000UL){
        if (serialAvailable()){
            recv_len += readResponseByteToBuffer();
            if (searchStrBuffer(test_str)){
                return SUCCESS_RESPONSE;
            } else if (searchStrBuffer(e_test_str)){
                start_time = millis();
                while(millis() - start_time < 100UL){
                    if(serialAvailable()){
                        recv_len += readResponseByteToBuffer();
                    }
                }
                char *str_buf = searchStrBuffer(": ");
                if (str_buf != NULL){
                    char err_code[8];
                    strcpy(err_code, str_buf + 2);
                    char *end_buf = strstr(err_code, "\r\n");
                    *end_buf = '\0';
                    errorCode = atoi(err_code);
                }
                return FIAL_RESPONSE;
            }
        } 
    }
    _atserial.println(rxBuffer);
    if (recv_len > 0){
        return UNKNOWN_RESPONSE;
    } else {
        return TIMEOUT_RESPONSE;
    }
}
Cmd_Response_t _BG96_Serial::waitForMQTTURC(unsigned int timeoutSeconds)
{
    // Hier hinterlegst du die exakten URCs,
    // wie sie der BG96 im MQTT-Kontext ausgibt.
    // Beispiel:

    unsigned long start_time = millis();
    unsigned int recv_len = 0;

    // Leere ggf. deinen internen Empfangs-Puffer, z. B. rxBuffer
    cleanBuffer();

    // Warte bis zu timeoutSeconds * 1000 ms
    while (millis() - start_time < (unsigned long)timeoutSeconds * 1000UL) {
        // Sobald Daten auf der seriellen Schnittstelle vorliegen:
        if (serialAvailable()) {
            // Lese byteweise und füge sie in rxBuffer ein
            recv_len += readResponseByteToBuffer();
            // Prüfe, ob +QMTRECV: gefunden wurde
            if (searchStrBuffer(MQTT_RECV_DATA)) {
                // => Dann gib SUCCESS_RESPONSE zurück
                return SUCCESS_RESPONSE;
            }
            // Prüfe, ob +QMTPUB: gefunden wurde
            else if (searchStrBuffer(MQTT_STATUS)) {
                // => Dann gib FIAL_RESPONSE zurück
                return FIAL_RESPONSE;
            }
        }
    }

    // Wenn wir hier ankommen, ist die Zeit abgelaufen (Timeout)
    // Falls wir Daten hatten (recv_len > 0), aber keines der beiden
    // gesuchten Muster gefunden haben, könnte man UNKNOWN_RESPONSE
    // zurückgeben. Hier unterscheiden wir explizit:
    if (recv_len > 0) {
        return UNKNOWN_RESPONSE;   // Daten da, aber nicht die richtigen URCs
    } else {
        return TIMEOUT_RESPONSE;   // Gar keine Daten
    }
}
/**
 * @brief Sendet einen AT-Befehl und sucht in der Antwort nach einem bestimmten Zeichen innerhalb eines Timeouts.
 *
 * Wiederholt den Vorgang bis zu dreimal bei Misserfolg.
 *
 * @param command Der zu sendende AT-Befehl.
 * @param test_chr Das zu suchende Zeichen.
 * @param timeout Timeout in Sekunden für die Antwortüberprüfung.
 * @return Status der Antwort (SUCCESS_RESPONSE, TIMEOUT_RESPONSE).
 */
Cmd_Response_t _BG96_Serial::sendAndSearchChr(const char *command, const char test_chr, unsigned int timeout)
{
    for (int i = 0; i < 3; i++){
        if(sendATcommand(command)){
            if (readResponseAndSearchChr(test_chr, timeout) == SUCCESS_RESPONSE){
                return SUCCESS_RESPONSE;
            }
        }
    }
    return TIMEOUT_RESPONSE;
}

/**
 * @brief Sendet einen AT-Befehl und sucht in der Antwort nach einer bestimmten Zeichenkette innerhalb eines Timeouts.
 *
 * Wiederholt den Vorgang bis zu dreimal bei Misserfolg.
 *
 * @param command Der zu sendende AT-Befehl.
 * @param test_str Die zu suchende Zeichenkette.
 * @param timeout Timeout in Sekunden für die Antwortüberprüfung.
 * @return Status der Antwort (SUCCESS_RESPONSE, TIMEOUT_RESPONSE).
 */
Cmd_Response_t _BG96_Serial::sendAndSearch(const char *command, const char *test_str, unsigned int timeout)
{
    for (int i = 0; i < 3; i++){
        if(sendATcommand(command)){
            if (readResponseAndSearch(test_str, timeout) == SUCCESS_RESPONSE){
                return SUCCESS_RESPONSE;
            }
        }
    }
    return TIMEOUT_RESPONSE;
}

/**
 * @brief Sendet einen AT-Befehl und sucht in der Antwort nach einer Erfolgskette oder einer Fehlerkette innerhalb eines Timeouts.
 *
 * Gibt den entsprechenden Status zurück.
 *
 * @param command Der zu sendende AT-Befehl.
 * @param test_str Die zu suchende Erfolgskette.
 * @param e_test_str Die zu suchende Fehlerkette.
 * @param timeout Timeout in Sekunden für die Antwortüberprüfung.
 * @return Status der Antwort (SUCCESS_RESPONSE, FIAL_RESPONSE, UNKNOWN_RESPONSE, TIMEOUT_RESPONSE).
 */
Cmd_Response_t _BG96_Serial::sendAndSearch(const char *command, const char *test_str, const char *e_test_str, unsigned int timeout)
{
    Cmd_Response_t resp_status = UNKNOWN_RESPONSE;
    for (int i = 0; i < 3; i++){
        if(sendATcommand(command)){
            resp_status = readResponseAndSearch(test_str, e_test_str, timeout);
            return resp_status;
        }
    }
    return resp_status;
}

/**
 * @brief Durchsucht den Empfangspuffer nach einer bestimmten Zeichenkette.
 *
 * @param test_str Die zu suchende Zeichenkette.
 * @return Zeiger auf die gefundene Zeichenkette im Puffer oder NULL, wenn nicht gefunden.
 */
char *_BG96_Serial::searchStrBuffer(const char *test_str)
{
    int buf_len = strlen((const char *)rxBuffer);
    if (buf_len < RX_BUFFER_LENGTH){
        return strstr((const char *)rxBuffer,test_str);
    } else {
        return NULL;
    }
}

/**
 * @brief Durchsucht den Empfangspuffer nach einem bestimmten Zeichen.
 *
 * @param test_chr Das zu suchende Zeichen.
 * @return Zeiger auf das gefundene Zeichen im Puffer oder NULL, wenn nicht gefunden.
 */
char *_BG96_Serial::searchChrBuffer(const char test_chr)
{
    int buf_len = strlen((const char *)rxBuffer);
    if (buf_len < RX_BUFFER_LENGTH){
        return strchr((const char *)rxBuffer, test_chr);
    } else {
        return NULL;
    }
}

/**
 * @brief Gibt den zuletzt erfassten Fehlercode zurück, falls vorhanden.
 *
 * @param s_err_code Referenz auf die Variable, in die der Fehlercode geschrieben wird.
 * @return true, wenn ein Fehlercode vorhanden ist, sonst false.
 */
bool _BG96_Serial::returnErrorCode(int &s_err_code)
{
    s_err_code = -1;
    if(errorCode != -1){
        s_err_code = errorCode;
        return true;
    }
    return false;
}

/**
 * @brief Bereinigt den Empfangspuffer, indem alle gespeicherten Daten gelöscht und der Pufferzeiger zurückgesetzt werden.
 */
void _BG96_Serial::cleanBuffer()
{
    memset(rxBuffer,'\0',RX_BUFFER_LENGTH);
    bufferHead = 0;
}

/**
 * @brief Überprüft, ob Daten auf der seriellen AT-Schnittstelle verfügbar sind.
 *
 * @return Anzahl der verfügbaren Bytes im Empfangspuffer.
 */
int _BG96_Serial::serialAvailable()
{
    unsigned int ret;
    ret = _atserial.available();
    return ret;
}
