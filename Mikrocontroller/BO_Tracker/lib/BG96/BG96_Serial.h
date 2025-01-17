// Die Klasse _BG96_Serial dient zur Kommunikation mit dem BG96-Modul über serielle Schnittstellen. 
// Sie bietet Methoden zur Konfiguration, Steuerung und Datenübertragung mittels AT-Befehlen.

#ifndef __BG96_SERIAL_H_
#define __BG96_SERIAL_H_

#include "string.h"
#include "stdlib.h"
#include "Arduino.h"
#include <TimeLib.h>
#include "BG96_ATCommand.h"
#include <ctime>

#define RX_BUFFER_LENGTH  1024
#define UART_DEBUG

const unsigned long Band_list[] = {
    9600,
    19200,
    38400,
    57600,
    115200,
    230400,
    460800,
    921600
};

typedef enum cmd_response{
    UNKNOWN_RESPONSE  = -2,
    TIMEOUT_RESPONSE  = -1,
    FIAL_RESPONSE  =  0,
    SUCCESS_RESPONSE  = 1,
}Cmd_Response_t;

class _BG96_Serial
{
  public:
    _BG96_Serial();

    ~_BG96_Serial();

    _BG96_Serial(Stream &atserial,Stream &dserial);

    void AT_bypass();

    template <class T>
    bool AutoATSerialBand(T &_atserial);

    bool sendDataAndCheck(const char *data_buf, const char *ok_str, const char *err_str, unsigned int timeout);

    bool sendATcommand(const char *command);

    unsigned int readResponseByteToBuffer();

    unsigned int readResponseToBuffer(unsigned int timeout);

    Cmd_Response_t readResponseAndSearch(const char *test_str, unsigned int timeout);

    Cmd_Response_t readResponseAndSearchChr(char test_chr, unsigned int timeout);

    Cmd_Response_t readResponseAndSearch(const char *test_str, const char *e_test_str, unsigned int timeout);

    Cmd_Response_t sendAndSearch(const char *command, const char *test_str, unsigned int timeout);

    Cmd_Response_t sendAndSearchChr(const char *command, const char test_chr, unsigned int timeout);

    Cmd_Response_t sendAndSearch(const char *command, const char *test_str, const char *e_test_str, unsigned int timeout);

    bool returnErrorCode(int &s_err_code);

    void cleanBuffer();

    int serialAvailable();

    char *searchStrBuffer(const char *test_str);

    char *searchChrBuffer(const char test_chr);
    Cmd_Response_t waitForMQTTURC(unsigned int timeoutSeconds);

    unsigned int bufferHead;
    char rxBuffer[RX_BUFFER_LENGTH];
    int errorCode = -1;

  private:
    Stream& _atserial;
    Stream& _dserial;


};

#endif