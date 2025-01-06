#ifndef __EVENTHANDLER_H_
#define __EVENTHANDLER_H_

#include "Tracker.h"

class EventHandler {
private:
    Tracker tracker;  // Attribut zur Speicherung der Eingabe-Daten
    JsonDocument &docOutput;
    JsonDocument &docInput;  // Attribut zur Speicherung der Ausgabe-Daten
    bool modemOff = false;
    Stream &DSerial;
public:
    // Konstruktor
    EventHandler(Stream &serial, Tracker &tracker, JsonDocument &docInput ,JsonDocument &docOutput);
    // Methoden
    void collectData();         // Handhabt den GNSS- und Tracker-Modus
    void dailyUpdates();
    void setMode(char *payload);
    void waitAndCheck();
    bool handleWakeUp();
};

#endif  // __MODE_HANDLE_H_
