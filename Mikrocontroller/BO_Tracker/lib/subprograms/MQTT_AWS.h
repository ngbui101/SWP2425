#ifndef __MQTT_AWS_H_
#define __MQTT_AWS_H_

#include "Modem.h"


class MQTT_AWS : public Modem {
protected:
    char mqtt_base_topic[32]; // Basis-Topic für MQTT-Kommunikation
    JsonDocument &docInput;
public:
    // Konstruktor
    MQTT_AWS(Stream &serial, _BG96_Module &modem,JsonDocument &docInput);

    // Methoden
    bool initMQTT();             // Initialisiert die MQTT-Verbindung
    bool startMQTT();            // Startet die MQTT-Verbindung
    bool publishData(unsigned long &pub_time, Mqtt_Qos_t MQTT_QoS, const char *subtopic); // Veröffentlicht Daten
    void handleMQTTStatusEvent(char *payload); // Behandelt Statusereignisse des MQTT-Clients
};

#endif // __MQTT_AWS_H_
