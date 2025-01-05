#ifndef __MQTT_AWS_H
#define __MQTT_AWS_H

#include "Modem.hpp"
#include "MQTT_Infos.h"

// MQTT
char mqtt_base_topic[32];

bool startMQTT(Stream &DSerial, _BG96_MQTT &_AWS){
  
  if (_AWS.OpenMQTTNetwork(MQTTIndex, MQTTServer, MQTTPort) != 0 )
  {
    DSerial.println("\r\nSet the MQTT Service Address Fail!");
    int e_code;
    if (_AWS.returnErrorCode(e_code))
    {
      DSerial.print("\r\nERROR CODE: ");
      DSerial.println(e_code);
      DSerial.println("Please check the documentation for error details.");
    }
  }
  DSerial.println("\r\nSet the MQTT Service Address Success!");

  DSerial.println("\r\nConfigure Timeout!");
  if (!_AWS.SetMQTTMessageTimeout(MQTTIndex, 10, 5, 1))
  {
    DSerial.println("\r\nMQTT Timeout Fail!");
    int e_code;
    if (_AWS.returnErrorCode(e_code))
    {
      DSerial.print("\r\nERROR CODE: ");
      DSerial.println(e_code);
      DSerial.println("Please check the documentation for error details.");
    }
  }

  DSerial.println("\r\nStart Create a MQTT Client!");
  if (_AWS.CreateMQTTClient(MQTTIndex, MQTTClientId, "", "") != 0)
  {
    DSerial.println("\r\nCreate a MQTT Client Fail!");
    int e_code;
    if (_AWS.returnErrorCode(e_code))
    {
      DSerial.print("\r\nERROR CODE: ");
      DSerial.println(e_code);
      DSerial.println("Please check the documentation for error details.");
    }
  }
  
  DSerial.println("\r\nCreate a MQTT Client Success!");

  DSerial.println("\r\nStart MQTT Subscribe Topic!");
  char mqtt_sub_topic[64];
  strcpy(mqtt_sub_topic, mqtt_base_topic);
  strcat(mqtt_sub_topic, "/sub");

  if (_AWS.MQTTSubscribeTopic(MQTTIndex, 1, mqtt_sub_topic, MQTT_QoS) != 0)
  {
    DSerial.println("\r\nMQTT Subscribe Topic Fail!");
    int e_code;
    if (_AWS.returnErrorCode(e_code))
    {
      DSerial.print("\r\nERROR CODE: ");
      DSerial.println(e_code);
      DSerial.println("Please check the documentation for error details.");
    }
  }
  DSerial.println("\r\nMQTT Subscribe Topic Success!");
  return true;
}

bool InitModemMQTT(Stream &DSerial, _BG96_MQTT &_AWS)
{
  // SSL Networking
  _AWS.DeleteCertificate("all");
  strcpy(mqtt_base_topic, "tracker/");
  strcat(mqtt_base_topic, ModemIMEI);
  
  char ssl_error[128];
  while (!_AWS.InitSSL(SSLIndex, aws_root_ca_pem, certificate_pem_crt, private_pem_key, ssl_error))
  {
    DSerial.println(ssl_error);
  }
  DSerial.println(ssl_error);

  // MQTT
  while (!_AWS.SetMQTTEnableSSL(MQTTIndex, SSLIndex, true))
  {
    DSerial.println("\r\nSetMQTTEnableSSL the MQTT Parameter Fail!");
    int e_code;
    if (_AWS.returnErrorCode(e_code))
    {
      DSerial.print("\r\nERROR CODE: ");
      DSerial.println(e_code);
      DSerial.println("Please check the documentation for error details.");
    }
  }
  DSerial.println("\r\nSetMQTTEnableSSL the MQTT Parameter Success!");

  DSerial.println("\r\nStart Config the MQTT Parameter!");
  while (!_AWS.SetMQTTConfigureParameters(MQTTIndex, PDPIndex, version, 150, SERVER_STORE_SUBSCRIPTIONS))
  {
    DSerial.println("\r\nConfig the MQTT Parameter Fail!");
    int e_code;
    if (_AWS.returnErrorCode(e_code))
    {
      DSerial.print("\r\nERROR CODE: ");
      DSerial.println(e_code);
      DSerial.println("Please check the documentation for error details.");
    }
  }
  DSerial.println("\r\nConfig the MQTT Parameter Success!");

  startMQTT(DSerial,_AWS);
  
  return true;
}


void handleMQTTStatusEvent(Stream &DSerial, _BG96_MQTT &_AWS, char *payload)
{
  char *sta_buf = strchr(payload, ',');
  if (atoi(sta_buf + 1) == 1)
  {
    if (_AWS.CloseMQTTClient(MQTTIndex))
    {
      DSerial.println("MQTT Client closed successfully!");
    }
  }
  else
  {
    DSerial.print("Status code: ");
    DSerial.println(atoi(sta_buf + 1));
  }
}
bool publishData(Stream &DSerial, _BG96_MQTT &_AWS, JsonDocument &docInput, unsigned long &pub_time, Mqtt_Qos_t MQTT_QoS, const char *subtopic)
{
  char payload[1028];
  serializeJsonPretty(docInput, payload);

  char mqtt_topic[64];
  strcpy(mqtt_topic, mqtt_base_topic);
  strcat(mqtt_topic, subtopic);

  int res = _AWS.MQTTPublishMessages(MQTTIndex, 1, MQTT_QoS, mqtt_topic, false, payload);

  if (res == PACKET_SEND_SUCCESS_AND_RECV_ACK || res == PACKET_RETRANSMISSION)
  {
    DSerial.println("Publish succeeded!");
    docInput.clear();
    pub_time = millis();
    return true;
  }
  else
  {
    DSerial.println("Publish failed!");
    return false;
  }
}

#endif
