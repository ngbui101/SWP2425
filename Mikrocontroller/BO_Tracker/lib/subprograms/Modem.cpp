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
    if(_BG96.FirstStart())
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

    _BG96.ConfigNetworks(trackerModes.RAT);
    char imei_tmp[64];

    if (_BG96.GetDevIMEI(imei_tmp))
    {
        String s = String(imei_tmp);
        s.trim();
        s.toCharArray(modemIMEI, 64);
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
bool Modem::setCurrentTimeToRTC()
{
    const char *modemTime = _BG96.GetCurrentTime();
    if (!setcurrentTime(modemTime))
    { // RTC im Board setzen
        initLogger.logError("SetTime");
        return false;
    }
    return true;
}

bool Modem::turnOnModem()
{
    if (!_BG96.InitModule())
    {   
        initLogger.logError("TurnOnModem");
        funkModuleEnable = false;
        return false;
    }
    // setCurrentTimeToRTC();
    funkModuleEnable = true;

    return true;
}

bool Modem::isModemAvailable()
{
    return funkModuleEnable;
}

_BG96_Module Modem::getModem()
{
    return this->_BG96;
}

bool Modem::startConnect()
{
    if (!_BG96.TurnOnInternet(PDPIndex))
    {
        runningLogger.logError("TurnOnInternet");
        connect = false;
        return false;
    }
    connect = true;
    return true;
}

bool Modem::isConnected()
{
    return connect;
}

void Modem::fillWithZero(Cell* arr[], int length, int maxSize) {
    if (length >= maxSize) return;  // Keine Auffüllung notwendig, wenn das Array bereits voll ist

    // Erstelle eine Dummy-Cell mit `CellID = 0`
    for (int i = length; i < maxSize; i++) {
        arr[i] = new Cell("FILLER", 0, 0, 0, 0, 0);  // Dynamische Erstellung einer Platzhalter-Zelle
    }
}

bool Modem::sortBySignal(Cell* arr[], int length) {
    if (arr == nullptr || length <= 1) {
        return false;  // Keine Sortierung notwendig oder ungültiger Array-Zeiger
    }

    // Bubble-Sort-Algorithmus (absteigend nach Signalstärke)
    for (int i = 0; i < length - 1; i++) {
        for (int j = 0; j < length - i - 1; j++) {
            if (arr[j]->getSignal() < arr[j + 1]->getSignal()) {
                Cell* temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }

    return true;  // Sortierung abgeschlossen
}


bool Modem::fillCellsQueue(){
    int MAX_CELLS = 6;
    Cell *cells[MAX_CELLS] = {nullptr};

    int countCell = _BG96.ScanCells(cells);
    // Serial.println("ScanCells OK");
    // Serial.println(countCell);
    if (countCell == 0)
        return false;
    if(countCell > 1){
        sortBySignal(cells,countCell);
    }

    fillWithZero(cells,countCell, MAX_CELLS);

    cells_queue.addAll(cells);

    // Serial.println("cells_queue OK");
    return true;
}