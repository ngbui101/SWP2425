#include <board.h>
#include <MQTT_AWS.hpp>
#include <GNSS.hpp>
#include <Battery.h>
#include <ArduinoJson.h>

#define DSerial SerialUSB
#define ATSerial Serial1

DynamicJsonDocument docInput(600);
DynamicJsonDocument docOutput(600);

char APN[] = "wm";
char LOGIN[] = "";
char PASSWORD[] = "";

char mqtt_server[] = "a336z3b6pu6hdu-ats.iot.us-east-1.amazonaws.com";
unsigned int mqtt_port = 8883;
char mqtt_clientId[] = "BG96";
char mqtt_topicName[] = "MyTopic";
unsigned int mqtt_index = 0;
Mqtt_Qos_t mqtt_qos = AT_MOST_ONCE;
unsigned long pub_time;

// IMEI of the modem
char IMEI[20];

_BG96_MQTT _AWS(ATSerial, DSerial);

_BG96_GNSS _GNSS(ATSerial, DSerial);

void setup()
{
  DSerial.begin(115200);
  delay(1000);
  while (DSerial.read() >= 0)
    ;
  DSerial.println("This is the _BG96 Debug Serial!");

  ATSerial.begin(115200);
  while (ATSerial.read() >= 0)
    ;
  delay(1000);

  _AWS.InitModule();
  _AWS.SetDevCommandEcho(false);
  _AWS.SetDevOutputformat(true);

  InitModemMQTT(_AWS, DSerial, APN, LOGIN, PASSWORD,
                mqtt_server, mqtt_port,
                mqtt_clientId, mqtt_topicName,
                AT_MOST_ONCE, mqtt_index,
                1, 2, IMEI);

  InitGNSS(_GNSS, DSerial, _GNSS.GetCurrentTime());
}

void loop()
{
  char payload[256];
  char *sta_buf;
  int res;
  char gnss_posi[128];

  DeserializationError error;
  
  if (!_GNSS.GetGNSSPositionInformation(gnss_posi))
  {
    DSerial.println("\r\nGet the GNSS Position Fail!");
    strcpy(gnss_posi, "no fix");
  }
  else
  {
    DSerial.println("\r\nGet the GNSS Position Success!");
    DSerial.println(gnss_posi);
  }

  Mqtt_URC_Event_t ret = _AWS.WaitCheckMQTTURCEvent(payload, 2);
  switch (ret)
  {
  case MQTT_RECV_DATA_EVENT:
    error = deserializeJson(docOutput, payload);

    if (error == DeserializationError::Ok)
    {
      if (docOutput["Device"] == "GNSS")
      {
        DSerial.println("Public GNSS Position: ");
        DSerial.println(docOutput["DeviceID"].as<String>());
        DSerial.println(docOutput["Timestamp"].as<String>());
        DSerial.println(docOutput["Device"].as<String>());
        DSerial.println(docOutput["OpCode"].as<String>());
        DSerial.println(docOutput["Position"].as<String>());
      }
      else
      {
        DSerial.println("Device not found!");
      }
    }
    else
    {
      DSerial.println("\r\n Error in  Deserialization!");
      DSerial.println(error.c_str());
    }

    break;

  case MQTT_STATUS_EVENT:
    sta_buf = strchr(payload, ',');
    if (atoi(sta_buf + 1) == 1)
    {
      if (_AWS.CloseMQTTClient(mqtt_index))
      {
        DSerial.println("\r\nClose the MQTT Client Success!");
      }
    }
    else
    {
      DSerial.print("\r\nStatus cade is :");
      DSerial.println(atoi(sta_buf + 1));
      DSerial.println("Please check the documentation for error details.");
    }
    break;

  default:
    break;
  }

  if (millis() - pub_time >= 5000UL)
  {
    pub_time = millis();
    docInput["DeviceID"] = IMEI;
    docInput["Timestamp"] = _GNSS.GetCurrentTime();
    docInput["Device"] = "BO-Tracker";
    docInput["OpCode"] = "Read";
    docInput["Position"] = gnss_posi;
    serializeJsonPretty(docInput, payload);

    res = _AWS.MQTTPublishMessages(mqtt_index, 1, AT_LEAST_ONCE, mqtt_topicName, false, payload);

    if (res == PACKET_SEND_SUCCESS_AND_RECV_ACK ||
        res == PACKET_RETRANSMISSION)
    {
      DSerial.println("Publish Succeded!");
    }
    else
    {
      DSerial.println("Publish failed!");
    }
  }
  delay(1000); // publish to topic every 1 seconds
}
