#ifndef _HTTP_H_
#define _HTTP_H_

#include "Modem.h"
#include "HTTP_Infos.h"

class HTTP : public Modem {
public:
    HTTP(Stream &atSerial, Stream &dSerial);
    ~HTTP();

    bool initHTTP();
    bool setHTTPURL(const char *url);
    bool sendPostRequest(char* payload);
    bool readResponse(char *recv_data);
    bool sendAndReadResponse(char* payload,char *recv_data);
};

#endif // _HTTP_H_
