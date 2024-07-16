#include "BG96_Serial.h"

// Konstruktor initialisiert die seriellen Schnittstellen
_BG96_Serial::_BG96_Serial() : _atserial(Serial), _dserial(Serial)
{
}

// Destruktor bereinigt den Puffer
_BG96_Serial::~_BG96_Serial()
{
    cleanBuffer();
}

// Konstruktor mit Parametern für die seriellen Schnittstellen
_BG96_Serial::_BG96_Serial(Stream &atserial, Stream &dserial) : _atserial(atserial), _dserial(dserial)
{
    _atserial.setTimeout(2000);
    _dserial.setTimeout(2000);
    cleanBuffer();
}

// Funktion für den direkten Durchgang der AT-Befehle
void _BG96_Serial::AT_bypass()
{
    while(_atserial.available()){
        _dserial.write(_atserial.read());
    }
    while(_dserial.available()){
        _atserial.write(_dserial.read());
    }
}

// Automatische Erkennung der AT-Serienbandbreite
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

// Senden von Daten und Überprüfung der Antwort
bool _BG96_Serial::sendDataAndCheck(const char *data_buf, const char *ok_str, const char *err_str, unsigned int timeout)
{
    delay(100);
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

// Senden eines AT-Befehls
bool _BG96_Serial::sendATcommand(const char *command)
{
    delay(100);
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

// Lesen eines Antwort-Bytes in den Puffer
unsigned int _BG96_Serial::readResponseByteToBuffer()
{
    char c = _atserial.read();
    rxBuffer[bufferHead] = c;
    bufferHead = (bufferHead + 1) % RX_BUFFER_LENGTH;
#if defined UART_DEBUG
    if (c == '\n'){
        _dserial.print(c);
        _dserial.print("<- ");
    }else {
        _dserial.print(c);
    }
#endif
    return 1;
}

// Lesen der Antwort in den Puffer innerhalb eines Zeitlimits
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

// Lesen der Antwort und Suche nach einem Zeichen innerhalb eines Zeitlimits
Cmd_Response_t _BG96_Serial::readResponseAndSearchChr(const char test_chr, unsigned int timeout)
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

// Lesen der Antwort und Suche nach einer Zeichenfolge innerhalb eines Zeitlimits
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

// Lesen der Antwort und Suche nach einer von zwei Zeichenfolgen innerhalb eines Zeitlimits
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
            }else if (searchStrBuffer(e_test_str)){
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
    if (recv_len > 0){
        return UNKNOWN_RESPONSE;
    } else {
        return TIMEOUT_RESPONSE;
    }
}

// Senden eines Befehls und Suche nach einem Zeichen in der Antwort
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

// Senden eines Befehls und Suche nach einer Zeichenfolge in der Antwort
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

// Senden eines Befehls und Suche nach einer von zwei Zeichenfolgen in der Antwort
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

// Suchen einer Zeichenfolge im Puffer
char *_BG96_Serial::searchStrBuffer(const char *test_str)
{
    int buf_len = strlen((const char *)rxBuffer);
    if (buf_len < RX_BUFFER_LENGTH){
        return strstr((const char *)rxBuffer,test_str);
    }else {
        return NULL;
    }
}

// Suchen eines Zeichens im Puffer
char *_BG96_Serial::searchChrBuffer(const char test_chr)
{
    int buf_len = strlen((const char *)rxBuffer);
    if (buf_len < RX_BUFFER_LENGTH){
        return strchr((const char *)rxBuffer, test_chr);
    }else {
        return NULL;
    }
}

// Rückgabe des Fehlercodes
bool _BG96_Serial::returnErrorCode(int &s_err_code)
{
    s_err_code = -1;
    if(errorCode != -1){
        s_err_code = errorCode;
        return true;
    }
    return false;
}

// Puffer bereinigen
void _BG96_Serial::cleanBuffer()
{
    memset(rxBuffer,'\0',RX_BUFFER_LENGTH);
    bufferHead = 0;
}

// Verfügbarkeit der seriellen Schnittstelle prüfen
int _BG96_Serial::serialAvailable()
{
    unsigned int ret;
    ret = _atserial.available();
    return ret;
}
