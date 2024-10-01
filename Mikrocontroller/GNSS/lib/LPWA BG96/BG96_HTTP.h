/*
 * A library for BG96 Development board
 * This file is about the BG96 HTTP function
 * 
 * Copyright (c) 
 * @Author       :
 * @Create time  :
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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