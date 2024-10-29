#include <board.h>
#include <MQTT_AWS.hpp>
#include <GNSS.hpp>
#include <Battery.h>
#include <ArduinoJson.h>

#define DSerial SerialUSB
#define ATSerial Serial1

StaticJsonDocument<600> docInput;
StaticJsonDocument<600> docOutput;

char APN[] = "internet.m2mportal.de";
// char APN[] = "iot.1nce.net";
// char APN[] = "wm";
char LOGIN[] = "";
char PASSWORD[] = "";

char mqtt_server[] = "a336z3b6pu6hdu-ats.iot.eu-central-1.amazonaws.com";
unsigned int mqtt_port = 8883;
char mqtt_clientId[] = "BG96";
// char mqtt_topicName[50];
char mqtt_sub_topic[64]; // Für "tracker/{IMEI}/sub"
char mqtt_pub_topic[64]; // Für "tracker/{IMEI}/pub"

unsigned int mqtt_index = 0;
Mqtt_Qos_t mqtt_qos = AT_MOST_ONCE;
unsigned long pub_time;

// GNSS
unsigned long gnss_start_time;
unsigned long last_millis = 0;
unsigned long time_taken;
bool gnssFixReceived = false;

// IMEI of the modem
char IMEI[20];
// mode
bool GnssMode = true;
unsigned int frequenz = 5000UL;

_BG96_MQTT _AWS(ATSerial, DSerial);
_BG96_GNSS _GNSS(ATSerial, DSerial);
_Battery _BoardBattery;
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
                mqtt_clientId, mqtt_sub_topic, mqtt_pub_topic,
                AT_MOST_ONCE, mqtt_index,
                1, 2, IMEI);

  InitGNSS(_GNSS, DSerial);
}

void loop()
{
  char payload[1028];
  char *sta_buf;
  int res;
  char gnss_posi[128];
  char gnss_nmea[128];
  char cell_infos[256];
  float temperature;
  float humid;
  float batterypercentage;

  DeserializationError error;

  Mqtt_URC_Event_t ret = _AWS.WaitCheckMQTTURCEvent(payload, 2);

  if (_AWS.ReportCellInformation("servingcell", cell_infos))
  {
    DSerial.println("Report Cell Information Successful");
  }
  batterypercentage = _BoardBattery.calculateBatteryPercentage();
  switch (ret)
  {
  case MQTT_RECV_DATA_EVENT:
    error = deserializeJson(docOutput, payload);

    if (error == DeserializationError::Ok)
    {
      if (docOutput["GnssMode"] == "true")
      {
        GnssMode = true;
        DSerial.println("GNSS Mode On!");
        gnss_start_time = millis();
      }
      else if (docOutput["GnssMode"] == "false")
      {
        GnssMode = false;
        DSerial.println("GNSS Mode Off!");
      }
      else
      {
        DSerial.println("Mode not found!");
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
  if (!_GNSS.GetGNSSPositionInformation(gnss_posi))
  {
    DSerial.println("\r\nGet the GNSS Position Fail!\nTry in 5 Seconds");
    strcpy(gnss_posi, "no fix");
    if (millis() - last_millis >= 5000u)
    {
      last_millis = millis();
    }
  }
  else
  {
    DSerial.println("\r\nGet the GNSS Position Success!");
    DSerial.println(gnss_posi);
    if (!gnssFixReceived)
    {
      time_taken = millis() - gnss_start_time;
      DSerial.print("Time taken to get GNSS Position: ");
      DSerial.print(time_taken);
      DSerial.println(" ms");
      gnssFixReceived = true;
    }
  }
  if (!_GNSS.GetGNSSNMEASentences(GPGGA, gnss_nmea))
  {
    DSerial.println("\r\nGet the GNSS NMEA Fail!");
  }
  else
  {
    DSerial.println(gnss_nmea);
  }
  if (GnssMode && (millis() - pub_time >= frequenz))
  {
    pub_time = millis();
    docInput["Timestamp"] = _GNSS.GetCurrentTime();
    docInput["CellInfos"] = cell_infos;
    docInput["Temperature"] = 23;
    docInput["Humidity"] = 90;
    docInput["BatteryPercentage"] = batterypercentage;
    docInput["Position"] = gnss_posi;
    docInput["NMEA"] = gnss_nmea;
    docInput["TimeToGetFirstFix"] = time_taken;

    serializeJsonPretty(docInput, payload);

    res = _AWS.MQTTPublishMessages(mqtt_index, 1, AT_LEAST_ONCE, mqtt_pub_topic, false, payload);

    if (res == PACKET_SEND_SUCCESS_AND_RECV_ACK ||
        res == PACKET_RETRANSMISSION)
    {
      DSerial.println("Publish with Succeded!");
    }
    else
    {
      DSerial.println("Publish failed!");
    }
  }
}