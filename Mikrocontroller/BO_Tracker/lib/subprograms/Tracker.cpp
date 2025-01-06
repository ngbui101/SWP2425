#include "Tracker.h"

// Konstruktor ruft den Basisklassen-Konstruktor (GNSS) auf
Tracker::Tracker(Stream &atSerial, Stream &dSerial, JsonDocument &doc)
    : GNSS(atSerial, dSerial, doc)
{
}

// Beispiel-Implementierung der InitModule()-Funktion
void Tracker::InitModule()
{
    DSerial.println("Beginne Initialisierung des Moduls...");
    
    if (initBoard())
    {
        DSerial.println("Board erfolgreich initialisiert.");
    }
    else
    {
        DSerial.println("Fehler bei der Initialisierung des Boards.");
        return;
    }
    // Schritt 1: Modem initialisieren
    if (initModem())
    {
        DSerial.println("Modem erfolgreich initialisiert.");
    }
    else
    {
        DSerial.println("Fehler bei der Initialisierung des Modems.");
        return;
    }

     if (setRTC())
    {
        DSerial.println("RTC erfolgreich vom Modem gesetzt.");
    }
    else
    {
        DSerial.println("Fehler: RTC konnte nicht vom Modem gesetzt werden.");
        return;
    }
    
    // Schritt 2: Board initialisieren

    // Schritt 3: GNSS initialisieren
    if (InitGNSS())
    {
        DSerial.println("GNSS erfolgreich initialisiert.");
    }
    else
    {
        DSerial.println("Fehler bei der Initialisierung von GNSS.");
        return;
    }
     // Schritt 4: MQTT initialisieren
    if (initMQTT())
    {
        DSerial.println("MQTT erfolgreich initialisiert.");
    }
    else
    {
        DSerial.println("Fehler bei der Initialisierung von MQTT.");
        return;
    }
    
    
    DSerial.println("Alle Module erfolgreich initialisiert.");
}


bool Tracker::setRTC()
{
    const char *modemTime = _BG96.GetCurrentTime();
    DSerial.println(modemTime); // Ausgabe der Modem-Zeit

    if (setupRTCFromModem(modemTime))
    { // RTC im Board setzen
        DSerial.println("RTC erfolgreich gesetzt!");
        return true;
    }
    else
    {
        DSerial.println("RTC konnte nicht vom Modem übernommen werden.");
        return false;
    }
}
