#include <Arduino.h>
#include <Wire.h> // I2C-Bibliothek einbinden

// I2C Adresse des Sensors (überprüfen Sie das Datenblatt)
#define HPP845E_ADDRESS 0x40 // Beispieladresse, überprüfen Sie das Datenblatt

// Funktionsprototypen
float readTemperature();
float readHumidity();

void setup() {
    Serial.begin(115200); // Serielle Kommunikation starten
    Wire.begin(); // I2C-Bus initialisieren
}

void loop() {
    // Temperatur und Luftfeuchtigkeit auslesen
    float temperature = readTemperature();
    float humidity = readHumidity();

    // Werte ausgeben
    Serial.print("Temperatur: ");
    Serial.print(temperature);
    Serial.println(" °C");
    Serial.print("Luftfeuchtigkeit: ");
    Serial.print(humidity);
    Serial.println(" %");

    delay(5000); // 5 Sekunden warten, bevor die nächste Messung erfolgt
}

// Funktion zur Temperaturmessung
float readTemperature() {
    Wire.beginTransmission(HPP845E_ADDRESS);
    Wire.write(0xE3); // Register für Temperatur (überprüfen Sie das Datenblatt)
    if (Wire.endTransmission() != 0) {
        Serial.println("Fehler beim Übertragen der Temperaturdaten!");
        return -999.0; // Fehlercode
    }

    Wire.requestFrom(HPP845E_ADDRESS, 2); // 2 Bytes anfordern
    if (Wire.available() == 2) {
        int16_t rawTemperature = (Wire.read() << 8) | Wire.read(); // Rohdaten lesen

        // Umwandlung in Temperatur in °C
        float temperature = (rawTemperature / 65536.0) * 175.72 - 46.85;
        return temperature; // Rückgabe in °C
    }
    Serial.println("Fehler beim Abrufen der Temperaturdaten!");
    return -999.0; // Fehlercode
}

// Funktion zur Luftfeuchtigkeitsmessung
float readHumidity() {
    Wire.beginTransmission(HPP845E_ADDRESS);
    Wire.write(0xE5); // Register für Luftfeuchtigkeit (überprüfen Sie das Datenblatt)
    if (Wire.endTransmission() != 0) {
        Serial.println("Fehler beim Übertragen der Luftfeuchtigkeit!");
        return -999.0; // Fehlercode
    }

    Wire.requestFrom(HPP845E_ADDRESS, 2); // 2 Bytes anfordern
    if (Wire.available() == 2) {
        int16_t rawHumidity = (Wire.read() << 8) | Wire.read(); // Rohdaten lesen

        // Umwandlung in relative Luftfeuchtigkeit
        float humidity = (rawHumidity / 65536.0) * 125 - 6;
        return humidity; // Rückgabe in %
    }
    Serial.println("Fehler beim Abrufen der Luftfeuchtigkeitsdaten!");
    return -999.0; // Fehlercode
}
