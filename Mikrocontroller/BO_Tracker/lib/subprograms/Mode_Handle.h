#ifndef __MODE_HANDLE_H_
#define __MODE_HANDLE_H_

#include "GNSS.h"

class Mode_Handle : public GNSS {
private:
    JsonDocument &docOutput;  // Attribut zur Speicherung der Ausgabe-Daten
    bool modemOff = false;
public:
    // Konstruktor
    Mode_Handle(Stream &serial, _BG96_Module &_BG96, JsonDocument &docInput, JsonDocument &docOutput);

    // Methoden
    void initModul();          // Initialisiert alle Module
    void modeHandle();         // Handhabt den GNSS- und Tracker-Modus
    void dailyUpdates();
    void eventHandle(char *payload);
    void waitAndCheck();
    bool handleWakeUp();
};

#endif  // __MODE_HANDLE_H_
