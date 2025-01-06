#ifndef __MQTT_AWS_H_
#define __MQTT_AWS_H_

#include "GNSS.h"


class MQTT_AWS : public GNSS {
protected:
    char mqtt_base_topic[32]; // Basis-Topic für MQTT-Kommunikation
public:
    // Konstruktor
    MQTT_AWS(Stream &atSerial, Stream &dSerial, JsonDocument &docInput);

    // Methoden
    bool initMQTT();             // Initialisiert die MQTT-Verbindung
    bool startMQTT();            // Startet die MQTT-Verbindung
    bool publishData(const char *subtopic); // Veröffentlicht Daten
    void handleMQTTStatusEvent(char *payload);
    
    void waitForResponse();
     // Behandelt Statusereignisse des MQTT-Clients
};

#endif // __MQTT_AWS_H_
