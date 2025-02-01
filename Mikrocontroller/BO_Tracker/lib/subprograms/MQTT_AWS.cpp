#include "MQTT_AWS.h"

// Konstruktor der MQTT_AWS-Klasse
MQTT_AWS::MQTT_AWS(Stream &atSerial, Stream &dSerial, JsonDocument &doc)
    : HTTP(atSerial, dSerial, doc)
{
}

// Initialisiert die MQTT-Verbindung
bool MQTT_AWS::initMQTT()
{
    // MQTT-Base-Topic mit IMEI erweitern
    strcpy(mqtt_base_topic, "tracker/");
    strcat(mqtt_base_topic, modemIMEI);

    if (!_BG96.SetMQTTConfigureParameters(MQTTIndex, PDPIndex, version, 180, SERVER_STORE_SUBSCRIPTIONS))
    {
        initLogger.logError("MQTT:Set_Parameters");
    }
    return true; // Startet MQTT-Verbindung
}

// Startet die MQTT-Verbindung
bool MQTT_AWS::startMQTT()
{
    _BG96.DeleteCertificate("all");
    strcpy(mqtt_base_topic, "tracker/");
    strcat(mqtt_base_topic, modemIMEI);
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

    if (_BG96.OpenMQTTNetwork(MQTTIndex, MQTTServer, MQTTPort) != 0)
    {
        initLogger.logError("MQTT:OpenNetwork");
    }

    if (!_BG96.SetMQTTMessageTimeout(MQTTIndex, 10, 5, 1))
    {
        initLogger.logError("MQTT:MessageTimeOut");
    }

    if (_BG96.CreateMQTTClient(MQTTIndex, MQTTClientId, "", "") != 0)
    {
        initLogger.logError("MQTT:CreateClient");
    }

    char mqtt_sub_topic[64];
    strcpy(mqtt_sub_topic, mqtt_base_topic);
    strcat(mqtt_sub_topic, "/sub");

    if (_BG96.MQTTSubscribeTopic(MQTTIndex, 1, mqtt_sub_topic, MQTT_QoS) != 0)
    {
        initLogger.logError("MQTT:SubscribeTopic");
    }

    mqtt_available = true;
    return true;
}

// Veröffentlicht Daten auf dem MQTT-Topic
bool MQTT_AWS::publishData(const char *subtopic)
{
    char payload[2056];
    serializeJsonPretty(docInput, payload);

    char mqtt_topic[64];
    strcpy(mqtt_topic, mqtt_base_topic);
    strcat(mqtt_topic, subtopic);

    int res = _BG96.MQTTPublishMessages(MQTTIndex, 1, MQTT_QoS, mqtt_topic, false, payload);

    if (res == PACKET_SEND_SUCCESS_AND_RECV_ACK)// || res == PACKET_RETRANSMISSION)
    {   
        docInput.clear();
        return true;
    }
    else
    {
        runningLogger.logError("MQTTPublish");
        mqtt_available = false;
        return false;
    }
}

// Behandelt Statusereignisse des MQTT-Clients
bool MQTT_AWS::handleMQTTStatusEventClose(char *payload)
{
    char *sta_buf = strchr(payload, ',');
    if (atoi(sta_buf + 1) == 1)
    {
        if (closeMQTTClient())
        {
            return true;
        }
        runningLogger.logError("closeMQTTClient");
    }
    return false;
}

bool MQTT_AWS::closeMQTTClient()
{
    if (!_BG96.CloseMQTTClient(MQTTIndex))
        return false;
    mqtt_available = false;
    return true;
}

Mqtt_Event_t MQTT_AWS::waitForResponse(char *response)
{
    Mqtt_URC_Event_t ret = _BG96.WaitCheckMQTTURCEvent(response, 1);
    switch (ret)
    {
    case MQTT_RECV_DATA_EVENT:
        return MQTT_RESPONSE;
    case MQTT_STATUS_EVENT:
        if (handleMQTTStatusEventClose(response))
            return MQTT_CLIENT_CLOSED;
    default:
        break;
    }
    return NONE;
}
