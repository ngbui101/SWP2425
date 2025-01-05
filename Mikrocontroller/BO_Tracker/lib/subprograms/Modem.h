#ifndef __MODEM_H_
#define __MODEM_H_

#include "BG96_Module.h"
#include "board.h"
#include "Network_Infos.h"

// Modem-Klasse zur Verwaltung der Modem-Operationen
class Modem : public _Board {
protected:
    // Private Attribute
    _BG96_Module &_BG96;  // Modem-Objekt (einzige Instanz)

    // Private Daten
    char modemIMEI[20]; // IMEI des Modems
    Cell *cells[6] = {nullptr}; // Zellinformationen

public:
    // Konstruktor
    Modem(Stream &serial, _BG96_Module &modem);

    // Methoden
    bool setRTC();             // Setzt die RTC des Boards
    bool startModem();         // Initialisiert das Modem
    bool initModem();          // Führt die vollständige Initialisierung des Modems durch
};

#endif  // __MODEM_H_
