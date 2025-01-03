#include "Board.h"
#include "arduino_bma456.h"
#include <ArduinoLowPower.h>

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

bool _Board::initBoard(Stream &DSerial)
{
    DSerial.println("Beginne Initialisierung des Boards...");

    // LED-Pin-Modus setzen
    pinMode(LED_BUILTIN, OUTPUT);
    DSerial.println("LED-Pin initialisiert.");

    // BMA456 initialisieren
    DSerial.println("Initialisiere BMA456...");
    bma456.initialize(RANGE_4G, ODR_50_HZ, NORMAL_AVG4, CIC_AVG);
    DSerial.println("BMA456 erfolgreich initialisiert.");

    // Batterie initialisieren
    DSerial.println("Initialisiere Batterie...");
    if (!initBattery())
    {
        DSerial.println("Fehler bei der Initialisierung der Batterie.");
        return false;
    }
    DSerial.println("Batterie erfolgreich initialisiert.");

    // Temperatur initialisieren
    DSerial.println("Initialisiere Temperatursensor...");
    if (!initTemp())
    {
        DSerial.println("Fehler bei der Initialisierung des Temperatursensors.");
        return false;
    }
    DSerial.println("Temperatursensor erfolgreich initialisiert.");

    // Wake-On-Motion aktivieren
    DSerial.println("Aktiviere Wake-On-Motion...");
    if (!(bma456.enableWakeOnMotion() == 0))
    {
        DSerial.println("Fehler bei der Aktivierung von Wake-On-Motion.");
        return false;
    }
    DSerial.println("Wake-On-Motion erfolgreich aktiviert.");
    return true; // Erfolgreich initialisiert
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
bool _Board::setupRTCFromModem(const char *modemTime)
{
    // Überprüfen, ob der String das richtige Format hat
    size_t len = strlen(modemTime);
    if (len < 17)
    {
        return false;
    }
    // Zeit und Datum aus dem String extrahieren
    char buf[3] = {0};
    buf[0] = modemTime[0];
    buf[1] = modemTime[1];
    int year = atoi(buf); // Jahr

    buf[0] = modemTime[3];
    buf[1] = modemTime[4];
    int month = atoi(buf); // Monat

    buf[0] = modemTime[6];
    buf[1] = modemTime[7];
    int day = atoi(buf); // Tag

    buf[0] = modemTime[9];
    buf[1] = modemTime[10];
    int hour = atoi(buf); // Stunde

    buf[0] = modemTime[12];
    buf[1] = modemTime[13];
    int minute = atoi(buf); // Minute

    buf[0] = modemTime[15];
    buf[1] = modemTime[16];
    int second = atoi(buf); // Sekunde

    // RTC initialisieren
    rtc.begin();

    // RTC mit extrahierten Werten konfigurieren
    rtc.setYear(year); // RTCZero erwartet Jahr ab 2000
    rtc.setMonth(month);
    rtc.setDay(day);
    rtc.setHours(hour);
    rtc.setMinutes(minute);
    rtc.setSeconds(second);

    return true;
}

/**
 * @brief Gibt die aktuelle Unix-Zeit (Epochenzeit) der RTC als Zeichenkette zurück.
 *
 * Diese Methode ruft die aktuelle Unix-Zeit von der RTC ab und konvertiert sie in einen
 * nullterminierten String.
 *
 * @return char* Ein statischer Puffer, der die Unix-Zeit als String enthält.
 */
char *_Board::getDateTime()
{
    // Statischer Puffer für die formattierte Zeichenkette
    static char buffer[30]; // Für "YYYY/MM/DD HH:MM:SS"

    // RTC-Werte auslesen
    uint16_t year = rtc.getYear() + 2000;
    uint8_t month = rtc.getMonth();
    uint8_t day = rtc.getDay();
    uint8_t hour = rtc.getHours();
    uint8_t minute = rtc.getMinutes();
    uint8_t second = rtc.getSeconds();

    // Datum und Zeit formatieren
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d:%02d",
             year, month, day, hour, minute, second);

    return buffer; // Rückgabe des Strings
}

bool _Board::waitWakeOnMotions()
{
    if (bma456.waitForMotion())
    {
        digitalWrite(LED_BUILTIN, HIGH); 
        delay(1000);                     
        digitalWrite(LED_BUILTIN, LOW);
        return true;
    }else return false;
}

bool _Board::checkOnMotionsfor10s(){
    return bma456.isMovementAboveThresholdFor10S(1000);
}
void _Board::deepSleep(int millis){
    if(millis <= 10000){
        LowPower.deepSleep(10000); 
    }
    LowPower.deepSleep(millis); 
}
