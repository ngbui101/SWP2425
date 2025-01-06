#include "Tracker.h"

// Konstruktor ruft den Basisklassen-Konstruktor (GNSS) auf
Tracker::Tracker(Stream &atSerial, Stream &dSerial, JsonDocument &doc)
    : MQTT_AWS(atSerial, dSerial, doc)
{
}

// Beispiel-Implementierung der InitModule()-Funktion
void Tracker::InitModule()
{
    initBoard();

    initModem();
    
    initHTTP();

    setRTC();

    InitGNSS();

    // initMQTT();

    DSerial.print("Total Error: ");
    int totalError = initLogger.getErrorCount();
    DSerial.println(totalError);
    char allErrorsBuffer[440]; 
    initLogger.getAllError(allErrorsBuffer);
    DSerial.println(allErrorsBuffer);
    initLogger.clear();
}


bool Tracker::setCurrentTimeToRTC()
{
    const char *modemTime = _BG96.GetCurrentTime();
    DSerial.println(modemTime); // Ausgabe der Modem-Zeit

    if (!setcurrentTime(modemTime))
    { // RTC im Board setzen
        initLogger.logError("SetTime");
        return false;
    }
    return true;
}
