#include "Board.h"

/**
 * @brief Konstruktor der Klasse _Board.
 *
 * Initialisiert ein Objekt der Klasse _Board. Aktuell keine spezifischen Aktionen.
 */
_Board::_Board()
{
}

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
    if (initBattery() && initTemp())
        return true; // Erfolgreich initialisiert
    else
        return false; // Fehler bei der Initialisierung
}

/**
 * @brief Konfiguriert die Echtzeituhr (RTC) basierend auf einem Zeitstempel vom Modem.
 *
 * Diese Methode analysiert einen Zeitstempel im Format "YYYY/MM/DD,HH:MM:SS+TZ"
 * und setzt die RTC entsprechend.
 *
 * @param modemTime Zeitstempel als `const char*` im Format "YYYY/MM/DD,HH:MM:SS+TZ".
 * @return true, wenn der Zeitstempel erfolgreich geparst und die RTC konfiguriert wurde.
 * @return false, wenn das Parsen des Zeitstempels fehlschlägt.
 */
bool _Board::setupRTCFromModem(const char* modemTime) {
    uint16_t year;
    uint8_t month, day, hour, minute, second;

    // Zeitstempel parsen und in einzelne Komponenten aufteilen
    if (sscanf(modemTime, "%4hu/%2hhu/%2hhu,%2hhu:%2hhu:%2hhu", &year, &month, &day, &hour, &minute, &second) == 6) {
        // RTC konfigurieren
        rtc.setYear(year);
        rtc.setMonth(month);
        rtc.setDay(day);
        rtc.setHours(hour);
        rtc.setMinutes(minute);
        rtc.setSeconds(second);
        return true; // Erfolgreich konfiguriert
    }

    return false; // Fehler beim Parsen des Zeitstempels
}

/**
 * @brief Gibt die aktuelle Unix-Zeit (Epochenzeit) der RTC als Zeichenkette zurück.
 *
 * Diese Methode ruft die aktuelle Unix-Zeit von der RTC ab und konvertiert sie in einen
 * nullterminierten String.
 *
 * @return char* Ein statischer Puffer, der die Unix-Zeit als String enthält.
 */
char* _Board::getDateTime() {
    uint32_t unixTime = rtc.getEpoch(); // Aktuellen Unix-Timestamp von der RTC abrufen

    // Statischer Puffer für die Zeichenkette (10 Ziffern + Nullterminierung)
    static char buffer[11]; 

    // Unix-Zeit in einen String umwandeln
    snprintf(buffer, sizeof(buffer), "%lu", unixTime);

    return buffer; // Rückgabe des Strings
}
