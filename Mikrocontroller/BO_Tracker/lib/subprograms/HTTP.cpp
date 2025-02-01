#include "HTTP.h"

HTTP::HTTP(Stream &atSerial, Stream &dSerial, JsonDocument &doc)
    : Modem(atSerial, dSerial, doc)
{
}

HTTP::~HTTP() {}

bool HTTP::initHTTP()
{
    if (!_BG96.SetHTTPConfigParameters(PDPIndex, false, false, http_type))
    {
        initLogger.logError("HTTP_Parameters");
        return false;
    }
    return true;
}

bool HTTP::setHTTPURL(const char *url)
{
    if (!_BG96.HTTPURL(url, WRITE_MODE))
    {
        initLogger.logError("HTTP_Set_URL");
        urlSetted = false;
        return false;
    }
    urlSetted = true;
    return true;
}

bool HTTP::sendPostRequest(char *payload)
{
    if (!_BG96.HTTPPOST(payload, 80))
    {
        urlSetted = false;
        runningLogger.logError("HTTPPOST");
        return false;
    }
    return true;
}

bool HTTP::readResponse(char *recv_data)
{
    if (!_BG96.HTTPRead(recv_data, 80))
    {
        urlSetted = false;
        runningLogger.logError("HTTPRead");
        return false;
    }
    return true;
}

bool HTTP::sendAndReadResponse(char *payload, char *recv_data){
    return (sendPostRequest(payload) && readResponse(recv_data));
}

bool HTTP::isUrlSetted()
{
    return urlSetted;
}