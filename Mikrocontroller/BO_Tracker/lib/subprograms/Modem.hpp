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

bool initModem(Stream &DSerial, _BG96_TCPIP &_Modem)
{
    if (_Modem.InitModule())
    {
        _Modem.SetDevOutputformat(true);
        _Modem.SetDevCommandEcho(false);
        DSerial.println("Init Module...");
        // _AWS.ConfigNetworks();
    }
    else
    {
        DSerial.println("Fail to Init Module...");
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
    return true;
}
#endif
