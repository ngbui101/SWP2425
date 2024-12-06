#ifndef __Modem_H
#define __Modem_H

#include "BG96_Module.h"
#include "TrackerMode.cpp"
#include <Board.h>
// APN
// char APN[] = "internet.m2mportal.de";
// char APN[] = "wm";
char APN[] = "iot.1nce.net";
char LOGIN[] = "";
char PASSWORD[] = "";
char ModemIMEI[20];

char RAT[] = "lte";
unsigned int PDPIndex = 1;
Cell *cells[6] = {nullptr};

bool setRTC(_BG96_TCPIP &_Modem, _Board &_ArdruinoZero)
{

    const char *modemTime = _Modem.GetCurrentTime();

    // const char *modemTime = "24/12/06,21:43:26+04";
    Serial.println(modemTime);
    
    if (_ArdruinoZero.setupRTCFromModem(modemTime))
        return true;
    else
        return false;
}

bool initModem(Stream &DSerial, _BG96_TCPIP &_Modem, _Board &_ArdruinoZero)
{
    if (_Modem.InitModule())
    {
        _Modem.SetDevOutputformat(true);
        _Modem.SetDevCommandEcho(false);
    }
    else
    {
        DSerial.println("Fail to Init Modem...");
        return false;
    }
    // IMEI
    char imei_tmp[64];

    if (_Modem.GetDevIMEI(imei_tmp))
    {
        String s = String(imei_tmp);
        s.trim();
        s.toCharArray(ModemIMEI, 64);
        DSerial.println(ModemIMEI);
    }
    char apn_error[64];
    if (!_Modem.InitAPNWithNetworkScanning(PDPIndex, APN, LOGIN, PASSWORD, apn_error, RAT, cells))
    {
        DSerial.println(apn_error);
        return false;
    }
    DSerial.println(apn_error);

    if(setRTC(_Modem, _ArdruinoZero)){
        DSerial.println("RTC Set from Modem");
    }else{
        DSerial.println("RTC Set from Modem failed");
    }

    return true;
}

#endif
