#ifndef __MQTT_AWS_H_
#define __MQTT_AWS_H_

#include "GNSS.h"
typedef enum mqtt_event
{
    MQTT_CLIENT_CLOSED = -1,
    NONE = 0,
    MQTT_RESPONSE = 1,
} Mqtt_Event_t;

class MQTT_AWS : public GNSS
{
protected:
    char mqtt_base_topic[32];
    bool mqtt_available = false;

public:
    // Konstruktor
    MQTT_AWS(Stream &atSerial, Stream &dSerial, JsonDocument &docInput);

    // Methoden
    bool initMQTT();                        // Initialisiert die MQTT-Verbindung
    bool startMQTT();                       // Startet die MQTT-Verbindung
    bool publishData(const char *subtopic); // Veröffentlicht Daten
    bool handleMQTTStatusEventClose(char *payload);

    Mqtt_Event_t waitForResponse(char *response);

    bool closeMQTTClient();
    // Behandelt Statusereignisse des MQTT-Clients
    bool isMQTTAvaliable();
};

#endif // __MQTT_AWS_H_
