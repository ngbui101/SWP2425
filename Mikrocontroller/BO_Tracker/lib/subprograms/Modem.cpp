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
    DSerial.println("Starte Modem...");
    if (_BG96.InitModule())
    {
        _BG96.SetDevOutputformat(true); // Setzt Ausgabeformat
        _BG96.SetDevCommandEcho(false); // Deaktiviert Kommandoecho
        _BG96.SaveSetting();            // Speichert Einstellungen
        DSerial.println("Modem gestartet!");
    }
    else
    {
        DSerial.println("Fehler: Modem konnte nicht gestartet werden.");
        return false;
    }
    return true;
}

// Führt die vollständige Initialisierung des Modems durch
bool Modem::initModem()
{
    DSerial.println("Beginne Modem-Initialisierung...");

    // Starte das Modem
    if (!startModem())
    {
        return false;
    }

    // Konfiguriere Netzwerkverbindung
    _BG96.ConfigNetworks(RAT);
    char imei_tmp[64];

    if (_BG96.GetDevIMEI(imei_tmp))
    {
        String s = String(imei_tmp);
        s.trim();
        s.toCharArray(trackerModes.modemIMEI, 64);
        DSerial.print("Modem IMEI: ");
        DSerial.println(trackerModes.modemIMEI);
    }
    else
    {
        DSerial.println("Fehler: Modem-IMEI konnte nicht abgerufen werden.");
        return false;
    }

    // Konfiguriere APN
    char apn_error[64];
    if (!_BG96.InitAPN(PDPIndex, APN, LOGIN, PASSWORD, apn_error))
    {
        DSerial.print("APN-Fehler: ");
        DSerial.println(apn_error);
        return false;
    }

    // Scanne Zellinformationen
    // _BG96.ScanCells(RAT, cells);
    DSerial.println("Zellinformationen gescannt.");

    DSerial.println("Modem-Initialisierung abgeschlossen!");
    return true;
}
