/*
*Die Klasse _BG96_HTTP implementiert HTTP-Funktionalitäten für das Quectel BG96-Modul, 
*einschließlich der Konfiguration von HTTP-Parametern, des Sendens von GET- und POST-Anfragen 
*und des Lesens von HTTP-Antworten.
 */

#ifndef __BG96_HTTP_H_
#define __BG96_HTTP_H_

#include "BG96_SSL.h"

typedef enum http_body_data_type{
    APPLICATION_X_WWW_FORM_URLENCODED = 0,
    TEXT_PLAIN = 1,
    APPLICATION_OCTET_STREAM = 2,
    MULTIPART_FORM_DATA = 3,
}HTTP_Body_Data_Type_t;

class _BG96_HTTP : public _BG96_SSL
{
  public:
    _BG96_HTTP();

    ~_BG96_HTTP();

    _BG96_HTTP(Stream &atserial, Stream &dserial);

    bool SetHTTPConfigParameters(unsigned int pdp_index, bool request_header, bool response_header, HTTP_Body_Data_Type_t type);

    bool SetHTTPEnableSSL(unsigned int ssl_index);

    bool HTTPURL(char* url, Cmd_Status_t status);

    bool HTTPGET(unsigned int timeout);

    bool HTTPPOST(char *post_data, unsigned int timeout);

    bool HTTTPPOSTFile(char *filename, unsigned int timeout);

    bool HTTPRead(char *read_data, unsigned int timeout);

    bool HTTPReadToFile(const char *filename, unsigned int timeout);

  private:

};

#endif