#include "Modem.h"

// Konstruktor der Klasse Modem
Modem::Modem(Stream &atSerial, Stream &dSerial)
    : _Board(dSerial), _BG96(atSerial, dSerial)
{
}

// Setzt die RTC vom Modem auf das Board
// Initialisiert das Modem (ohne Netzwerkkonfiguration)
bool Modem::startModem()
{
    if (_BG96.InitModule())
    {
        _BG96.SetDevOutputformat(true); // Set output format
        _BG96.SetDevCommandEcho(false); // Disable command echo
        _BG96.SaveSetting();            // Save settings
    }
    else
    {
        initLogger.logError("ModemStart");
        return false;
    }
    return true;
}

bool Modem::initModem()
{
    if (!startModem())
    {
        return false;
    }

    _BG96.ConfigNetworks(RAT);
    char imei_tmp[64];

    if (_BG96.GetDevIMEI(imei_tmp))
    {
        String s = String(imei_tmp);
        s.trim();
        s.toCharArray(trackerModes.modemIMEI, 64);
    }
    else
    {
        initLogger.logError("ModemIMEI");
        return false;
    }

    char apn_error[64];
    if (!_BG96.InitAPN(PDPIndex, APN, LOGIN, PASSWORD, apn_error))
    {
        initLogger.logError("APN");
        return false;
    }

    // _BG96.ScanCells(RAT, cells); // Uncomment if needed

    return true;
}
