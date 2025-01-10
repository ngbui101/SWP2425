#include "Board.h"

/**
 * @brief Konstruktor der Klasse _Board.
 *
 * Initialisiert ein Objekt der Klasse _Board. Aktuell keine spezifischen Aktionen.
 */
_Board::_Board(Stream& serial) : _Lowpower(), DSerial(serial) {
}

_Board::~_Board(){}
/**
 * @brief Initialisiert die Hardwarekomponenten des Boards.
 *
 * Diese Methode ruft `initBattery()` und `initTemp()` auf, um die Batterie- und Temperatursensoren zu initialisieren.
 *
 * @return true, wenn sowohl die Batterie- als auch die Temperatursensorinitialisierung erfolgreich sind.
 * @return false, wenn eine der Initialisierungen fehlschlägt.
 */

bool _Board::initBoard()
{

    initialize(RANGE_4G, ODR_50_HZ, NORMAL_AVG4, CIC_AVG);

    if (!initBattery()) {
        initLogger.logError("Battery");
    }

    if (!initTemp()) {
        initLogger.logError("Temperature");
    }

    if (enableWakeOnMotion() != 0) {
        initLogger.logError("WakeOnMotion");
    }

    return initLogger.getErrorCount() == 0;
}

