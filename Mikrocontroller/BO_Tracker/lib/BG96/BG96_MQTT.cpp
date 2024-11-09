#include "BG96_MQTT.h"

/**
 * @brief Konstruktor der Klasse _BG96_MQTT.
 *
 * Initialisiert ein Objekt der Klasse ohne Parameter.
 */
_BG96_MQTT::_BG96_MQTT() 
{
}

/**
 * @brief Destruktor der Klasse _BG96_MQTT.
 *
 * Bereinigt Ressourcen, die während der Laufzeit belegt wurden.
 */
_BG96_MQTT::~_BG96_MQTT() 
{
}

/**
 * @brief Konstruktor mit Übergabe der seriellen Schnittstellen.
 *
 * @param atserial Serielle Schnittstelle für AT-Kommandos.
 * @param dserial Serielle Schnittstelle für Debugging.
 */
_BG96_MQTT::_BG96_MQTT(Stream &atserial, Stream &dserial) : _BG96_SSL(atserial, dserial) 
{
}

/**
 * @brief Konfiguriert die optionalen Parameter des MQTT-Clients.
 *
 * Diese Methode nutzt den AT-Befehl AT+QMTCFG, um verschiedene MQTT-Parameter
 * wie Version, PDP-Index, Keep-Alive und Session-Type zu setzen.
 * 
 * @param mqtt_index Index des MQTT-Clients (0-5).
 * @param pdp_index PDP-Kontext-Index (1-16).
 * @param version MQTT-Version (3 = v3.1, 4 = v3.1.1).
 * @param keep_alive_time Keep-Alive-Zeit in Sekunden (0-3600).
 * @param type Sitzungstyp (0 = Clean Session, 1 = Persistent Session).
 * @return true bei erfolgreicher Konfiguration, false bei Fehler.
 *
 * @see Quectel BG96 MQTT Application Note, Abschnitt 3.3.1 (Seite 9)
 */
bool _BG96_MQTT::SetMQTTConfigureParameters(unsigned int mqtt_index, unsigned int pdp_index, Mqtt_Version_t version, unsigned int keep_alive_time, Mqtt_Session_Type_t type)
{
    char cmd[32], buf[32];
    
    // MQTT-Version konfigurieren
    strcpy(cmd, MQTT_CONFIG_PARAMETER);
    sprintf(buf, "=\"version\",%d,%d", mqtt_index, version);
    strcat(cmd, buf);
    if(!sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        return false;
    }
    
    // PDP-Kontext konfigurieren
    memset(cmd, '\0', 32);
    memset(buf, '\0', 32);
    strcpy(cmd, MQTT_CONFIG_PARAMETER);
    sprintf(buf, "=\"pdpcid\",%d,%d", mqtt_index, pdp_index);
    strcat(cmd, buf);
    if(!sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        return false;
    }

    // Keep-Alive-Zeit konfigurieren
    memset(cmd, '\0', 32);
    memset(buf, '\0', 32);
    strcpy(cmd, MQTT_CONFIG_PARAMETER);
    sprintf(buf, "=\"keepalive\",%d,%d", mqtt_index, keep_alive_time);
    strcat(cmd, buf);
    if(!sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        return false;
    }

    // Sitzungstyp konfigurieren
    memset(cmd, '\0', 32);
    memset(buf, '\0', 32);
    strcpy(cmd, MQTT_CONFIG_PARAMETER);
    sprintf(buf, "=\"session\",%d,%d", mqtt_index, type);
    strcat(cmd, buf);
    if(!sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        return false;
    }

    return true;
}

/**
 * @brief Aktiviert oder deaktiviert die Verwendung von SSL für MQTT.
 *
 * Mit diesem AT-Befehl kann festgelegt werden, ob SSL für den MQTT-Client verwendet werden soll.
 * 
 * @param mqtt_index Index des MQTT-Clients (0-5).
 * @param ssl_index SSL-Kontext-Index (0-5).
 * @param enable true, um SSL zu aktivieren; false, um SSL zu deaktivieren.
 * @return true bei erfolgreicher Konfiguration, false bei Fehler.
 *
 * @see Quectel BG96 MQTT Application Note, Abschnitt 3.3.1 (Seite 11)
 */
bool _BG96_MQTT::SetMQTTEnableSSL(unsigned int mqtt_index, unsigned int ssl_index, bool enable)
{
    char cmd[32], buf[16];
    strcpy(cmd, MQTT_CONFIG_PARAMETER);
    if(enable){
        sprintf(buf, "=\"ssl\",%d,1,%d", mqtt_index, ssl_index);
    }else {
        sprintf(buf, "=\"ssl\",%d,0,%d", mqtt_index, ssl_index);
    }
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        return true;
    }
    return false;
}

/**
 * @brief Setzt das Timeout und die Anzahl der Wiederholungen für MQTT-Nachrichten.
 *
 * Mit dieser Methode werden Timeout-Einstellungen und Wiederholungsversuche für MQTT-Nachrichten festgelegt.
 * 
 * @param mqtt_index Index des MQTT-Clients (0-5).
 * @param pkt_timeout Zeitüberschreitung für Pakete in Sekunden (1-60).
 * @param retry_num Anzahl der Wiederholungen bei Timeout (0-10).
 * @param timeout_notice true, um Timeout-Meldungen zu aktivieren, false um sie zu deaktivieren.
 * @return true bei erfolgreicher Konfiguration, false bei Fehler.
 *
 * @see Quectel BG96 MQTT Application Note, Abschnitt 3.3.1 (Seite 12)
 */
bool _BG96_MQTT::SetMQTTMessageTimeout(unsigned int mqtt_index, unsigned int pkt_timeout, unsigned int retry_num, bool timeout_notice)
{
    char cmd[32], buf[32];
    strcpy(cmd, MQTT_CONFIG_PARAMETER);
    if(timeout_notice){
        sprintf(buf, "=\"timeout\",%d,%d,%d,1", mqtt_index, pkt_timeout, retry_num);
    }else {
        sprintf(buf, "=\"timeout\",%d,%d,%d,0", mqtt_index, pkt_timeout, retry_num);
    }
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        return true;
    }
    return false;
}

/**
 * @brief Öffnet eine Netzwerkverbindung für den MQTT-Client.
 *
 * Diese Methode nutzt den AT-Befehl AT+QMTOPEN, um eine Verbindung zum MQTT-Server herzustellen.
 * 
 * @param mqtt_index Index des MQTT-Clients (0-5).
 * @param host_name Name oder IP-Adresse des MQTT-Servers.
 * @param port Port des MQTT-Servers (0-65535).
 * @return Ergebnis des Verbindungsaufbaus als Mqtt_Network_Result_t.
 *
 * @see Quectel BG96 MQTT Application Note, Abschnitt 3.3.2 (Seite 15)
 */
Mqtt_Network_Result_t _BG96_MQTT::OpenMQTTNetwork(unsigned int mqtt_index, char *host_name, unsigned int port)
{
    char cmd[128], buf[128];
    strcpy(cmd, MQTT_OPEN_NETWORK);
    sprintf(buf, "=%d,\"%s\",%d", mqtt_index, host_name, port);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, MQTT_OPEN_NETWORK, RESPONSE_ERROR, 150)){
        unsigned long start_time = millis();
        while(millis() - start_time < 200UL){
            if(serialAvailable()){
                readResponseByteToBuffer();
            }
        }
        char *sta_buf = searchChrBuffer(',');
        return (Mqtt_Network_Result_t)atoi(sta_buf + 1);
    }
    return (Mqtt_Network_Result_t)-2;
}

/**
 * @brief Schließt die Netzwerkverbindung für den MQTT-Client.
 *
 * Diese Methode nutzt den AT-Befehl AT+QMTCLOSE, um die Verbindung zum MQTT-Server zu trennen.
 * 
 * @param mqtt_index Index des MQTT-Clients (0-5).
 * @return true bei erfolgreicher Schließung, false bei Fehler.
 *
 * @see Quectel BG96 MQTT Application Note, Abschnitt 3.3.2 (Seite 16)
 */
bool _BG96_MQTT::CloseMQTTNetwork(unsigned int mqtt_index)
{
    char cmd[16], buf[8];
    strcpy(cmd, MQTT_CLOSE_NETWORK);
    sprintf(buf, "=%d", mqtt_index);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, MQTT_CLOSE_NETWORK, RESPONSE_ERROR, 2)){
        unsigned long start_time = millis();
        while(millis() - start_time < 200UL){
            if(serialAvailable()){
                readResponseByteToBuffer();
            }
        }
        char *sta_buf = searchChrBuffer(',');
        if(atoi(sta_buf + 1) == 0){
            return true;
        }
    }
    return false;
}

/**
 * @brief Erstellt einen MQTT-Client.
 *
 * Diese Methode verwendet den AT-Befehl AT+QMTCONN, um einen MQTT-Client zu erstellen und ihn mit einem MQTT-Server zu verbinden.
 * 
 * @param mqtt_index Index des MQTT-Clients (0-5).
 * @param client_id ID des MQTT-Clients.
 * @param username Benutzername für die MQTT-Authentifizierung (optional).
 * @param password Passwort für die MQTT-Authentifizierung (optional).
 * @return Status der MQTT-Client-Erstellung als Mqtt_Client_Result_Status_t.
 *
 * @see Quectel BG96 MQTT Application Note, Abschnitt 3.3.3 (Seite 17)
 */
Mqtt_Client_Result_Status_t _BG96_MQTT::CreateMQTTClient(unsigned int mqtt_index, const char *client_id, const char *username, const char *password)
{
    char cmd[128], buf[128];
    strcpy(cmd, MQTT_CREATE_CLIENT);
    // Benutzername und Passwort nur hinzufügen, wenn sie nicht leer sind
    if (strcmp(username, "") != 0 && strcmp(password, "") != 0){
        sprintf(buf, "=%d,\"%s\",\"%s\",\"%s\"", mqtt_index, client_id, username, password);
    } else {
        sprintf(buf, "=%d,\"%s\"", mqtt_index, client_id);
    }
    strcat(cmd, buf);

    if (sendAndSearch(cmd, MQTT_CREATE_CLIENT, RESPONSE_ERROR, 150)){
        unsigned long start_time = millis();
        while((millis() - start_time) < 200UL) {
            if(serialAvailable()) {
                readResponseByteToBuffer();
            }
        }
        char temp[16];
        char *sta_buf = searchChrBuffer(',');
        strcpy(temp, sta_buf + 1);
        sta_buf = strchr(temp, ',');
        *sta_buf = '\0';
        return (Mqtt_Client_Result_Status_t)atoi(temp);
    }
    return (Mqtt_Client_Result_Status_t)-1;
}

/**
 * @brief Schließt den MQTT-Client.
 *
 * Diese Methode verwendet den AT-Befehl AT+QMTDISC, um den MQTT-Client zu schließen.
 * 
 * @param mqtt_index Index des MQTT-Clients (0-5).
 * @return true bei erfolgreicher Schließung, false bei Fehler.
 *
 * @see Quectel BG96 MQTT Application Note, Abschnitt 3.3.3 (Seite 18)
 */
bool _BG96_MQTT::CloseMQTTClient(unsigned int mqtt_index)
{
    char cmd[16], buf[8];
    strcpy(cmd, MQTT_CLOSE_CLIENT);
    sprintf(buf, "=%d", mqtt_index);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, MQTT_CLOSE_CLIENT, RESPONSE_OK, 2)){
        unsigned long start_time = millis();
        while(millis() - start_time < 200UL){
            if(serialAvailable()){
                readResponseByteToBuffer();
            }
        }
        char *sta_buf = searchChrBuffer(',');
        if(atoi(sta_buf + 1) == 0){
            return true;
        }
    }
    return false;
}

/**
 * @brief Abonniert ein MQTT-Topic.
 *
 * Diese Methode nutzt den AT-Befehl AT+QMTSUB, um ein MQTT-Topic zu abonnieren.
 * 
 * @param mqtt_index Index des MQTT-Clients (0-5).
 * @param msg_id Nachrichts-ID für das Abonnement.
 * @param topic MQTT-Topic, das abonniert werden soll.
 * @param qos QoS-Level für das Abonnement (0 = At Most Once, 1 = At Least Once, 2 = Exactly Once).
 * @return Status des Abonnements als Mqtt_Client_Result_Status_t.
 *
 * @see Quectel BG96 MQTT Application Note, Abschnitt 3.3.4 (Seite 19)
 */
Mqtt_Client_Result_Status_t _BG96_MQTT::MQTTSubscribeTopic(unsigned int mqtt_index, unsigned int msg_id, char *topic, Mqtt_Qos_t qos)
{
    char cmd[512], buf[512];
    strcpy(cmd, MQTT_SUBSCRIBE_TOPICS);
    sprintf(buf, "=%d,%d,\"%s\",%d", mqtt_index, msg_id, topic, qos);
    strcat(cmd, buf);
    if (sendAndSearch(cmd, MQTT_SUBSCRIBE_TOPICS, RESPONSE_ERROR, 150)){
        unsigned long start_time = millis();
        while(millis() - start_time < 200UL){
            if(serialAvailable()){
                readResponseByteToBuffer();
            }
        }
        int i = 0;
        char temp[16], *p[4];
        char *sta_buf = searchChrBuffer(',');
        strcpy(temp, sta_buf + 1);
        p[0] = strtok(temp, ",");
        while(p[i] != NULL && i < 3){ // Array-Grenzen prüfen
            i++;
            p[i] = strtok(NULL, ",");
        }
        p[i] = NULL;
        return (Mqtt_Client_Result_Status_t)atoi(p[1]);
    }
    return (Mqtt_Client_Result_Status_t)-1;
}

/**
 * @brief Kündigt das Abonnement eines MQTT-Topics.
 *
 * Diese Methode nutzt den AT-Befehl AT+QMTUNS, um ein zuvor abonniertes Topic abzubestellen.
 * 
 * @param mqtt_index Index des MQTT-Clients (0-5).
 * @param msg_id Nachrichts-ID des Abonnements.
 * @param topic MQTT-Topic, das abbestellt werden soll.
 * @return Status der Kündigung als Mqtt_Client_Result_Status_t.
 *
 * @see Quectel BG96 MQTT Application Note, Abschnitt 3.3.4 (Seite 20)
 */
Mqtt_Client_Result_Status_t _BG96_MQTT::MQTTUnsubscribeTopic(unsigned int mqtt_index, unsigned int msg_id, char *topic)
{
    char cmd[40], buf[32];
    strcpy(cmd, MQTT_UNSUBSCRIBE_TOPICS);
    sprintf(buf, "=%d,%d,\"%s\"", mqtt_index, msg_id, topic);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, MQTT_UNSUBSCRIBE_TOPICS, RESPONSE_ERROR, 150)){
        unsigned long start_time = millis();
        while(millis() - start_time < 200UL){
            if(serialAvailable()){
                readResponseByteToBuffer();
            }
        }
        char temp[16];
        char *sta_buf = searchChrBuffer(',');
        strcpy(temp, sta_buf + 1);
        sta_buf = strchr(temp, ',');
        *sta_buf = '\0';
        return (Mqtt_Client_Result_Status_t)atoi(temp);
    }
    return (Mqtt_Client_Result_Status_t)-1;
}

/**
 * @brief Veröffentlicht eine Nachricht auf einem MQTT-Topic.
 *
 * Diese Methode nutzt den AT-Befehl AT+QMTPUB, um eine Nachricht auf einem Topic zu veröffentlichen.
 * 
 * @param mqtt_index Index des MQTT-Clients (0-5).
 * @param msg_id Nachrichts-ID.
 * @param qos QoS-Level für die Veröffentlichung (0 = At Most Once, 1 = At Least Once, 2 = Exactly Once).
 * @param topic MQTT-Topic, auf dem veröffentlicht werden soll.
 * @param retain true, um die Nachricht persistent zu machen, false, um sie nicht zu behalten.
 * @param publish_data Die zu veröffentlichende Nachricht.
 * @return Status der Veröffentlichung als Mqtt_Client_Result_Status_t.
 *
 * @see Quectel BG96 MQTT Application Note, Abschnitt 3.3.5 (Seite 21)
 */
Mqtt_Client_Result_Status_t _BG96_MQTT::MQTTPublishMessages(unsigned int mqtt_index, unsigned int msg_id, Mqtt_Qos_t qos, char *topic, bool retain, char *publish_data)
{
    char cmd[512], buf[128];
    strcpy(cmd, MQTT_PUBLISH_MESSAGES);
    if(retain){
        sprintf(buf, "=%d,%d,%d,1,\"%s\"", mqtt_index, msg_id, qos, topic);
    } else {
        sprintf(buf, "=%d,%d,%d,0,\"%s\"", mqtt_index, msg_id, qos, topic);
    }
    strcat(cmd, buf);
    if (sendAndSearchChr(cmd, '>', 2)){
        char ctrl_z[] = { (char)0x1A, '\0' };
        strcat(publish_data, ctrl_z);
        if (sendDataAndCheck(publish_data, MQTT_PUBLISH_MESSAGES, RESPONSE_ERROR, 150)){
            unsigned long start_time = millis();
            while(millis() - start_time < 500UL){
                if(serialAvailable()){
                    readResponseByteToBuffer();
                }
            }
            memset(publish_data, '\0', strlen(publish_data));
            char temp[16];
            char *sta_buf = searchChrBuffer(',');
            strcpy(temp, sta_buf + 1);
            sta_buf = strchr(temp, ',');
            *sta_buf = '\0';
            return (Mqtt_Client_Result_Status_t)atoi(temp);
        }
    }
    return (Mqtt_Client_Result_Status_t)-1;
}

/**
 * @brief Überprüft auf eingehende MQTT-Events.
 *
 * Diese Methode wartet auf MQTT-Unsolicited Response Codes (URC) und überprüft die Ereignisse.
 * 
 * @param event Pointer auf den Speicher, in den das empfangene Ereignis geschrieben wird.
 * @param timeout Timeout in Sekunden für die Überprüfung.
 * @return Der Typ des empfangenen Ereignisses als Mqtt_URC_Event_t.
 *
 * @see Quectel BG96 MQTT Application Note, Abschnitt 3.3.6 (Seite 22)
 */
Mqtt_URC_Event_t _BG96_MQTT::WaitCheckMQTTURCEvent(char *event, unsigned int timeout)
{
    Cmd_Response_t ret = readResponseAndSearch(MQTT_RECV_DATA, MQTT_STATUS, timeout);
    
    unsigned long start_time = millis();
    while (millis() - start_time < 200UL){
        if (serialAvailable()){
            readResponseByteToBuffer();
        }
    }
    if (ret == SUCCESS_RESPONSE){
        char *sta_buf = searchStrBuffer(": ");
        // Daten im JSON-Format empfangen
        char *JsonData = strchr(sta_buf + 2, '{');		
        strcpy(event, JsonData);		
        return MQTT_RECV_DATA_EVENT;
    } else if (ret == FIAL_RESPONSE){
        char *sta_buf = searchStrBuffer(": ");
        strcpy(event, sta_buf + 2);
        return MQTT_STATUS_EVENT;
    }
    return (Mqtt_URC_Event_t)-1;
}
