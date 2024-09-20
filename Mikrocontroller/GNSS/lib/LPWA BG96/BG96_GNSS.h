/*
 * A library for BG96 Development board
 * This file is about the BG96 GNSS function
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

#ifndef __BG96_GNSS_H_
#define __BG96_GNSS_H_

#include "BG96_HTTP.h"

typedef enum gnss_work_mode{
    STAND_ALONE = 1,
    MS_BASED = 2,
    MS_ASSISTED = 3,
    SPEED_OPTIMAL = 4,
}GNSS_Work_Mode_t;

typedef enum gnss_constellation{
    GPS_ONLY = 0,
    GPS_GLONASS_BEIDOU_GALILEO = 1,
    GPS_GLONASS_BEIDOU = 2,
    GPS_GLONASS_GALILEO = 3,
    GPS_GLONASS = 4,
    GPS_BEIDOU_GALILEO = 5,
    GPS_GALILEO = 6,
}GNSS_Constellation_t;

typedef enum gnss_output_port{
	NOPORT = 1,
	USBNMEA = 2,
	UARTNMEA = 3,
}GNSS_OutputPort_t;

typedef enum nmea_sentences_type {
	GPGGA = 1,
	GPRMC = 2,
	GPGSV = 4,
	GPGSA = 8,
	GPVTG = 16,
}NMEA_Type_t;

class _BG96_GNSS : public _BG96_HTTP
{
  public:
    _BG96_GNSS();

    ~_BG96_GNSS();

    _BG96_GNSS(Stream &atserial, Stream &dserial);

    bool SetGNSSConstellation(GNSS_Constellation_t constellation);

    bool SetGNSSEnableNMEASentences(bool enable);

    bool SetGNSSAutoRun(bool auto_run);

    bool TurnOnGNSS(GNSS_Work_Mode_t mode, Cmd_Status_t status);

    bool GetGNSSPositionInformation(char *position);

    bool TurnOffGNSS();

    bool GetGNSSNMEASentences(NMEA_Type_t type, char *sentences);
	
    bool SetGNSSOutputPort(GNSS_OutputPort_t outport);

    bool EnableGpsOneXTRA();

    bool InjectGpsOneXTRATime(const char* timey);

    bool InjectGpsOneXTRAData(const char* filename);

    bool IsGpsOneXtraDataUptoDate();

    bool UpdateGpsOneXtraData();

    bool InitGpsOneXTRA();
   private:
   ///GPSOneXTRA Update Liste
    const char* xtra_links[3] = {
        "http://xtrapath1.izatcloud.net/xtra2.bin",
        "http://xtrapath2.izatcloud.net/xtra2.bin",
        "http://xtrapath3.izatcloud.net/xtra2.bin"
    };
};

#endif