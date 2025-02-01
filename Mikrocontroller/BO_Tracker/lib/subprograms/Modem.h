#ifndef __MODEM_H_
#define __MODEM_H_

#include "BG96_Module.h"
#include "board.h"
#include "Network_Infos.h"
#include "MQTT_Infos.h"
#include "CELL_FIFO.h"


// Modem-Klasse zur Verwaltung der Modem-Operationen
class Modem : public _Board {
protected:
    // Das interne _BG96_Module-Objekt
    _BG96_Module _BG96;
    char modemIMEI[20]; 
    bool funkModuleEnable = false;
    bool connect = false;
    JsonDocument &docInput;
    
    // Cell *cells[6] = {nullptr};
    CellFIFO cells_queue;
    
public:
    // Konstruktor (nur zwei Streams)
    Modem(Stream &atSerial, Stream &dSerial, JsonDocument &docInput);

    // Methoden
    bool startModem();         // Initialisiert das Modem
    bool initModem();
    bool turnOnModem();
    
    bool setCurrentTimeToRTC(); 

    bool isModemAvailable();

    bool startConnect();
    
    bool isConnected();

    bool sortBySignal(Cell* arr[], int length);

    void fillWithZero(Cell* arr[], int length, int maxSize);

    bool fillCellsQueue();

    bool handleCellInfosMode();

    _BG96_Module getModem();
private:
    int MAX_CELLS = 6;
};

#endif  // __MODEM_H_
