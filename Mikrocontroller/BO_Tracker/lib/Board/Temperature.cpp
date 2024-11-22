#include <Temperature.h>

_Temperature::_Temperature() {}
_Temperature::~_Temperature() {}

bool _Temperature::initTemp(){
    Wire.begin(); 
    return true;
}

bool _Temperature::isMeasurementComplete() {
    delay(50); // Für Temperaturmessung ausreichend
    return true; // Wenn keine Fehler auftreten, gehe davon aus, dass die Messung abgeschlossen ist
}

float _Temperature::readHumidity() {
    Wire.beginTransmission(HPP845E_ADDRESS);
    Wire.write(0xF5); // Befehl für Luftfeuchtigkeit
    Wire.endTransmission();

    // Polling oder Blockieren der Messung mit Verzögerung
    if (isMeasurementComplete()) {
        Wire.requestFrom(HPP845E_ADDRESS, 2);
        if (Wire.available() == 2) {
            int16_t rawHumidity = (Wire.read() << 8) | Wire.read(); // Rohdaten lesen
            // Umwandlung in relative Luftfeuchtigkeit
            return (rawHumidity / 65536.0) * 125 - 6;
        }
    }
    return -999.0; // Fehlercode
}

float _Temperature::readTemperature() {
    Wire.beginTransmission(HPP845E_ADDRESS);
    Wire.write(0xF3); // Befehl für Temperatur
    Wire.endTransmission();

    // Polling oder Blockieren der Messung mit Verzögerung
    if (isMeasurementComplete()) {
        Wire.requestFrom(HPP845E_ADDRESS, 2);
        if (Wire.available() == 2) {
            int16_t rawTemperature = (Wire.read() << 8) | Wire.read(); // Rohdaten lesen
            // Umwandlung in Temperatur in °C
            return (rawTemperature / 65536.0) * 175.72 - 46.85;
        }
    }
    return -999.0; // Fehlercode
}