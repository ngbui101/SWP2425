#include "Modem.h"

// Konstruktor der Klasse Modem
Modem::Modem(Stream &serial, _BG96_Module &modem)
    : _Board(serial), _BG96(modem) {}

// Setzt die RTC vom Modem auf das Board
bool Modem::setRTC() {
    const char *modemTime = _BG96.GetCurrentTime();
    DSerial.println(modemTime);  // Ausgabe der Modem-Zeit

    if (setupRTCFromModem(modemTime)) {  // RTC im Board setzen
        DSerial.println("RTC erfolgreich gesetzt!");
        return true;
    } else {
        DSerial.println("RTC konnte nicht vom Modem übernommen werden.");
        return false;
    }
}

// Initialisiert das Modem (ohne Netzwerkkonfiguration)
bool Modem::startModem() {
    DSerial.println("Starte Modem...");
    if (_BG96.InitModule()) {
        _BG96.SetDevOutputformat(true);  // Setzt Ausgabeformat
        _BG96.SetDevCommandEcho(false);  // Deaktiviert Kommandoecho
        _BG96.SaveSetting();             // Speichert Einstellungen
        DSerial.println("Modem gestartet!");
    } else {
        DSerial.println("Fehler: Modem konnte nicht gestartet werden.");
        return false;
    }
    return true;
}

// Führt die vollständige Initialisierung des Modems durch
bool Modem::initModem() {
    DSerial.println("Beginne Modem-Initialisierung...");

    // Starte das Modem
    if (!startModem()) {
        return false;
    }

    // Konfiguriere Netzwerkverbindung
    _BG96.ConfigNetworks(RAT);
    char imei_tmp[64];

    if (_BG96.GetDevIMEI(imei_tmp)) {
        String s = String(imei_tmp);
        s.trim();
        s.toCharArray(modemIMEI, 64);
        DSerial.print("Modem IMEI: ");
        DSerial.println(modemIMEI);
    } else {
        DSerial.println("Fehler: Modem-IMEI konnte nicht abgerufen werden.");
        return false;
    }

    // Konfiguriere APN
    char apn_error[64];
    if (!_BG96.InitAPN(PDPIndex, APN, LOGIN, PASSWORD, apn_error)) {
        DSerial.print("APN-Fehler: ");
        DSerial.println(apn_error);
        return false;
    }

    // Scanne Zellinformationen
    _BG96.ScanCells(RAT, cells);
    DSerial.println("Zellinformationen gescannt.");

    // RTC einstellen
    if (setRTC()) {
        DSerial.println("RTC erfolgreich vom Modem gesetzt.");
    } else {
        DSerial.println("Fehler: RTC konnte nicht vom Modem gesetzt werden.");
        return false;
    }

    DSerial.println("Modem-Initialisierung abgeschlossen!");
    return true;
}
