#include "BG96_HTTP.h"

_BG96_HTTP::_BG96_HTTP()
{

}

_BG96_HTTP::~_BG96_HTTP()
{

}

_BG96_HTTP::_BG96_HTTP(Stream &atserial, Stream &dserial) : _BG96_SSL(atserial, dserial)
{

}

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

bool _BG96_HTTP::HTTPURL(char* url, Cmd_Status_t status)
{
    char cmd[32], buf[16];
    strcpy(cmd, HTTP_SET_URL);
    if(status == READ_MODE){
        strcat(cmd, "?");
        if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
            char *sta_buf = searchStrBuffer(": ");
            if(sta_buf == NULL){
                url[0] = '\0';  // Set the buffer to an empty string
            } else {
                char *end_buf = searchStrBuffer("\r\nOK");
                if (end_buf != NULL) {
                    *end_buf = '\0';
                    // Copy the URL from the response buffer to the output buffer
                    strcpy(url, sta_buf + 2);
                } else {
                    url[0] = '\0';  // Set to empty string if end marker not found
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

bool _BG96_HTTP::HTTPPOST(char *post_data, unsigned int timeout)
{
    char cmd[32], buf[16];
    strcpy(cmd, HTTP_POST_REQUEST);
    sprintf(buf, "=%d,%d,%d", strlen(post_data), timeout, timeout);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_CONNECT, RESPONSE_ERROR, 10)){
        if(sendDataAndCheck(post_data, HTTP_POST_REQUEST, RESPONSE_ERROR, timeout)){
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
    }
    return false;
}

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
        }else {
            errorCode = atoi(sta_buf + 2);
        }
    }
    return false;
}

bool _BG96_HTTP::HTTPRead(char *read_data, unsigned int timeout)
{
    char cmd[32], buf[16];
    strcpy(cmd, HTTP_READ_RESPONSE);
    sprintf(buf, "=%d", timeout);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, HTTP_READ_RESPONSE, RESPONSE_ERROR, timeout)){
        unsigned long start_time = millis();
        errorCode = -1;
        while(millis() - start_time < 200UL){
            if(serialAvailable()){
                readResponseByteToBuffer();
            }
        }
        char *sta_buf = searchStrBuffer(RESPONSE_CONNECT);
        char *end_buf = searchStrBuffer("OK\r\n\r\n");
        *end_buf = '\0';
        strcpy(read_data, sta_buf + strlen(RESPONSE_CONNECT) + 2);
        return true;
    }
    return false;
}

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
        }else {
            errorCode = atoi(sta_buf + 2);
        }
    }
    return false;
}