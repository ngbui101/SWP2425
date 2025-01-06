#include "HTTP.h"

HTTP::HTTP(Stream &atSerial, Stream &dSerial)
    : Modem(atSerial, dSerial)
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

bool HTTP::setHTTPURL()
{
    if (!_BG96.HTTPURL(http_url, WRITE_MODE))
    {
        initLogger.logError("HTTP_Set_URL");
        return false;
    }
    return true;
}

bool HTTP::sendPostRequest(char *payload)
{
    if (!_BG96.HTTPPOST(payload, 80))
    {
        return false;
    }
    return true;
}

bool HTTP::readResponse(char *recv_data)
{
    if (!_BG96.HTTPRead(recv_data, 80))
    {
        return false;
    }
    return true;
}

bool HTTP::sendAndReadResponse(char *payload, char *recv_data){
    return (sendPostRequest(payload) && readResponse(recv_data));
}

