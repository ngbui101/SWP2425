#ifndef __Modem_H
#define __Modem_H

#include "BG96_Module.h"
#include "TrackerMode.cpp"
#include <Board.h>
#include "Network_Infos.h"
// APN
// char APN[] = "internet.m2mportal.de";
// char SUPURL[]  = "supl.google.com:7276";
// char APN[] = "wm";
// char APN[] = "iot.1nce.net";
// char LOGIN[] = "";
// char PASSWORD[] = "";
char ModemIMEI[20];
// char RAT[] = "gsm";
// unsigned int PDPIndex = 1;
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

bool startModem(Stream &DSerial, _BG96_TCPIP &_Modem){
    if (_Modem.InitModule())
    {
        _Modem.SetDevOutputformat(true);
        _Modem.SetDevCommandEcho(false);
        _Modem.SaveSetting();
    }
    else
    {
        DSerial.println("Fail to Init Modem...");
        return false;
    }
    return true;
}

bool initModem(Stream &DSerial, _BG96_TCPIP &_Modem, _Board &_ArdruinoZero)
{
    startModem(DSerial, _Modem);
    // IMEI
    _Modem.ConfigNetworks(RAT);
    char imei_tmp[64];

    if (_Modem.GetDevIMEI(imei_tmp))
    {
        String s = String(imei_tmp);
        s.trim();
        s.toCharArray(ModemIMEI, 64);
        DSerial.println(ModemIMEI);
    }
    char apn_error[64];

    if (!_Modem.InitAPN(PDPIndex, APN, LOGIN, PASSWORD, apn_error))
    {
        DSerial.println(apn_error);
        return false;
    }
    _Modem.ScanCells(RAT,cells);
    DSerial.println(apn_error);
    if(setRTC(_Modem, _ArdruinoZero)){
        DSerial.println("RTC Set from Modem");
    }else{
        DSerial.println("RTC Set from Modem failed");
    }

    return true;
}

#endif
