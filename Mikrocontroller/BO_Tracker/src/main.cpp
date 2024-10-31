#include <board.h>
#include <MQTT_AWS.hpp>
#include <GNSS.hpp>
#include <Battery.h>
#include <ArduinoJson.h>

#define DSerial SerialUSB
#define ATSerial Serial1

DynamicJsonDocument docInput(600);
DynamicJsonDocument docOutput(600);

char APN[] = "internet.m2mportal.de";
// char APN[] = "iot.1nce.net";
// char APN[] = "wm";
char LOGIN[] = "";
char PASSWORD[] = "";

char mqtt_server[] = "a336z3b6pu6hdu-ats.iot.eu-central-1.amazonaws.com";
unsigned int mqtt_port = 8883;
char mqtt_clientId[] = "BG96";
char mqtt_sub_topic[64];
char mqtt_pub_topic[64];

unsigned int mqtt_index = 0;
Mqtt_Qos_t mqtt_qos = AT_MOST_ONCE;
unsigned long pub_time;

bool gnssFixReceived = false;

bool GnssMode = false;
bool CellInfosMode = false;
bool NmeaMode = false;
bool BatteryMode = false;

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
                1, 2);

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
  float batterypercentage;

  DeserializationError error;

  Mqtt_URC_Event_t ret = _AWS.WaitCheckMQTTURCEvent(payload, 2);

  switch (ret)
  {
    case MQTT_RECV_DATA_EVENT:
      error = deserializeJson(docOutput, payload);

      if (error == DeserializationError::Ok)
      {
        GnssMode = (docOutput["GnssMode"] == "1");
        CellInfosMode = (docOutput["CellInfosMode"] == "1");
        NmeaMode = (docOutput["NmeaMode"] == "1");
        BatteryMode = (docOutput["BatteryMode"] == "1");

        if (docOutput["frequenz"].is<unsigned int>())
        {
          unsigned int newFrequenz = docOutput["frequenz"];
          if (newFrequenz > 0)
          {
            frequenz = newFrequenz;
            DSerial.print("Updated publishing frequency to: ");
            DSerial.println(frequenz);
          }
        }
      }
      else
      {
        DSerial.println("\r\n Error in Deserialization!");
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
        DSerial.print("\r\nStatus code is :");
        DSerial.println(atoi(sta_buf + 1));
        DSerial.println("Please check the documentation for error details.");
      }
      break;

    default:
      break;
  }

  if (millis() - pub_time >= frequenz)
  {
    pub_time = millis();
    if (GnssMode)
    {
      _GNSS.GetGNSSPositionInformation(gnss_posi);
      docInput["Position"] = gnss_posi;
    }

    if (NmeaMode)
    {
      _GNSS.GetGNSSNMEASentences(GPGGA, gnss_nmea);
      docInput["NMEA"] = gnss_nmea;
    }
    if (BatteryMode)
    {
      batterypercentage = _BoardBattery.calculateBatteryPercentage();
      docInput["BatteryPercentage"] = batterypercentage;
    }
    if (CellInfosMode)
    {
      _AWS.ReportCellInformation(const_cast<char*>("servingcell"), cell_infos);
      docInput["CellInfos"] = cell_infos;
    }

    docInput["Timestamp"] = _GNSS.GetCurrentTime();
    serializeJsonPretty(docInput, payload);

    res = _AWS.MQTTPublishMessages(mqtt_index, 1, AT_LEAST_ONCE, mqtt_pub_topic, false, payload);

    if (res == PACKET_SEND_SUCCESS_AND_RECV_ACK ||
        res == PACKET_RETRANSMISSION)
    {
      DSerial.println("Publish succeeded!");
      docInput.clear();
    }
    else
    {
      DSerial.println("Publish failed!");
    }
  }
}
