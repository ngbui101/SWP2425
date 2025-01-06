#include "MQTT_AWS.h"

// Konstruktor der MQTT_AWS-Klasse
MQTT_AWS::MQTT_AWS(Stream &atSerial, Stream &dSerial, JsonDocument &doc)
    : GNSS(atSerial, dSerial,doc)
{
}

// Initialisiert die MQTT-Verbindung
bool MQTT_AWS::initMQTT()
{   
    // MQTT-Base-Topic mit IMEI erweitern
    strcpy(mqtt_base_topic, "tracker/");
    strcat(mqtt_base_topic, trackerModes.modemIMEI);
    _BG96.DeleteCertificate("all");

    char ssl_error[128];
    if (!_BG96.InitSSL(SSLIndex, aws_root_ca_pem, certificate_pem_crt, private_pem_key, ssl_error))
    {
        initLogger.logError("MQTT:InitSSL");
    }

    if (!_BG96.SetMQTTEnableSSL(MQTTIndex, SSLIndex, true))
    {
        initLogger.logError("MQTT:SetSSL");
    }
    if (!_BG96.SetMQTTConfigureParameters(MQTTIndex, PDPIndex, version, 150, SERVER_STORE_SUBSCRIPTIONS))
    {
        initLogger.logError("MQTT:Set_Parameters");
    }
    return true; // Startet MQTT-Verbindung
}

// Startet die MQTT-Verbindung
bool MQTT_AWS::startMQTT()
{
    if (_BG96.OpenMQTTNetwork(MQTTIndex, MQTTServer, MQTTPort) != 0)
    {
        DSerial.println("Fehler: MQTT-Netzwerk konnte nicht geöffnet werden!");
        return false;
    }
    DSerial.println("MQTT-Netzwerk erfolgreich geöffnet!");

    if (!_BG96.SetMQTTMessageTimeout(MQTTIndex, 10, 5, 1))
    {
        DSerial.println("Fehler: MQTT-Timeout konnte nicht gesetzt werden!");
        return false;
    }

    if (_BG96.CreateMQTTClient(MQTTIndex, MQTTClientId, "", "") != 0)
    {
        DSerial.println("Fehler: MQTT-Client konnte nicht erstellt werden!");
        return false;
    }

    DSerial.println("MQTT-Client erfolgreich erstellt!");

    char mqtt_sub_topic[64];
    strcpy(mqtt_sub_topic, mqtt_base_topic);
    strcat(mqtt_sub_topic, "/sub");

    if (_BG96.MQTTSubscribeTopic(MQTTIndex, 1, mqtt_sub_topic, MQTT_QoS) != 0)
    {
        DSerial.println("Fehler: MQTT-Topic konnte nicht abonniert werden!");
        return false;
    }

    DSerial.println("MQTT-Topic erfolgreich abonniert!");
    return true;
}

// Veröffentlicht Daten auf dem MQTT-Topic
bool MQTT_AWS::publishData(const char *subtopic)
{      
    char payload[1028];
    serializeJsonPretty(docInput, payload);

    char mqtt_topic[64];
    strcpy(mqtt_topic, mqtt_base_topic);
    strcat(mqtt_topic, subtopic);

    int res = _BG96.MQTTPublishMessages(MQTTIndex, 1, MQTT_QoS, mqtt_topic, false, payload);

    if (res == PACKET_SEND_SUCCESS_AND_RECV_ACK || res == PACKET_RETRANSMISSION)
    {
        DSerial.println("Veröffentlichung erfolgreich!");
        docInput.clear();
        return true;
    }
    else
    {
        DSerial.println("Fehler bei der Veröffentlichung!");
        return false;
    }
}

// Behandelt Statusereignisse des MQTT-Clients
void MQTT_AWS::handleMQTTStatusEvent(char *payload)
{
    char *sta_buf = strchr(payload, ',');
    if (atoi(sta_buf + 1) == 1)
    {
        if (_BG96.CloseMQTTClient(MQTTIndex))
        {
            DSerial.println("MQTT-Client erfolgreich geschlossen!");
        }
    }
    else
    {
        DSerial.print("Statuscode: ");
        DSerial.println(atoi(sta_buf + 1));
    }
}

void MQTT_AWS::waitForResponse(){
    
}