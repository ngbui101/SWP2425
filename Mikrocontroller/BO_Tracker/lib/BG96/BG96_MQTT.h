/*
*Die Klasse BG96_MQTT stellt eine Implementierung der MQTT-Protokollsteuerung für 
*das Quectel BG96 Modul dar und bietet eine Reihe von Methoden, um das Modul für 
*die Kommunikation mit einem MQTT-Server zu konfigurieren, zu verbinden, Nachrichten 
*zu veröffentlichen und Topics zu abonnieren. Diese Dokumentation beschreibt die 
*wesentlichen Methoden der Klasse und deren Funktionen im Detail.
* Mehr: quectel_bg96_mqtt_application_note_v1-2.pdf
 */

#ifndef __BG96_MQTT_H_
#define __BG96_MQTT_H_

#include "BG96_HTTP.h"

typedef enum mqtt_version{
    MQTT_V4 = 4,
    MQTT_V3 = 3,
}Mqtt_Version_t;

typedef enum mqtt_qos{
    AT_MOST_ONCE = 0,
    AT_LEAST_ONCE = 1,
    EXACTLY_ONCE = 2,
}Mqtt_Qos_t;

typedef enum mqtt_session_type{
    SERVER_STORE_SUBSCRIPTIONS = 0,
    SERVER_DISCARD_INFORMATION = 1,
}Mqtt_Session_Type_t;

typedef enum mqtt_network_result_event{
    MQTT_NETWORK_OPEN_FAIL = -1,
    MQTT_NETWORK_OPEN_SUCCESS = 0,
    MQTT_NETWORK_WRONG_PARAMETER = 1,
    MQTT_NETWORK_INDEX_OCCUPIED = 2,
    MQTT_NETWORK_ACTIVATE_PDP_FAIL = 3,
    MQTT_NETWORK_PARSE_DOMAIN_FAIL = 4,
    MQTT_NETWORK_DISCONNECTION_ERROR = 5,
}Mqtt_Network_Result_t;

typedef enum mqtt_client_result_status{
    PACKET_SEND_SUCCESS_AND_RECV_ACK = 0,
    PACKET_RETRANSMISSION = 1,
    PACKET_SEND_FAIL = 2,
}Mqtt_Client_Result_Status_t;

typedef enum mqtt_urc_event{
    MQTT_STATUS_EVENT = 0,
    MQTT_RECV_DATA_EVENT = 1,
}Mqtt_URC_Event_t;

class _BG96_MQTT : public _BG96_HTTP
{
  public:
    _BG96_MQTT();

    ~_BG96_MQTT();

    _BG96_MQTT(Stream &atserial, Stream &dserial);

    bool SetMQTTConfigureParameters(unsigned int mqtt_index, unsigned int pdp_index, Mqtt_Version_t version, unsigned int keep_alive_time, Mqtt_Session_Type_t type);

    bool SetMQTTEnableSSL(unsigned int mqtt_index, unsigned int ssl_index, bool enable);

    bool SetMQTTMessageTimeout(unsigned int mqtt_index, unsigned int pkt_timeout, unsigned int retry_num, bool timeout_notice);

    bool SetMQTTAlibabaCloudDeviceInformation(unsigned int mqtt_index, char *product_key, char *device_name, char *device_secret);

    Mqtt_Network_Result_t OpenMQTTNetwork(unsigned int mqtt_index,const char *host_name, unsigned int port);

    bool CloseMQTTNetwork(unsigned int mqtt_index);

    Mqtt_Client_Result_Status_t CreateMQTTClient(unsigned int mqtt_index, const char *client_id, const char *username, const char *password);

    bool CloseMQTTClient(unsigned int mqtt_index);

    Mqtt_Client_Result_Status_t MQTTSubscribeTopic(unsigned int mqtt_index, unsigned int msg_id, char *topic, Mqtt_Qos_t qos);

    Mqtt_Client_Result_Status_t MQTTUnsubscribeTopic(unsigned int mqtt_index, unsigned int msg_id, char *topic);

    Mqtt_Client_Result_Status_t MQTTPublishMessages(unsigned int mqtt_index, unsigned int msg_id, Mqtt_Qos_t qos, char *topic, bool retain, char *publish_data);

    Mqtt_URC_Event_t WaitCheckMQTTURCEvent(char *event, unsigned int timeout);

  private:

};

#endif