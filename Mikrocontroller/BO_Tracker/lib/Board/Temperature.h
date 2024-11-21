#ifndef _TEMPERATURE_H_  
#define _TEMPERATURE_H_

#include <Arduino.h>
#include <Wire.h> // I2C-Bibliothek einbinden

// I2C Adresse des Sensors (überprüfen Sie das Datenblatt)
#define HPP845E_ADDRESS 0x40 // Beispieladresse, überprüfen Sie das Datenblatt

class _Temperature {
public:
    _Temperature();    // Konstruktor öffentlich
    ~_Temperature();   // Destruktor öffentlich
    bool initTemp();
    float readTemperature();
    float readHumidity();
private:
    bool isMeasurementComplete();
    // Hier kannst du private Member-Variablen deklarieren, falls erforderlich
};
#endif 