#include <MQTT_AWS.hpp>
#include <GNSS.hpp>
#include <Battery.h>
#include <ArduinoJson.h>

#define DSerial SerialUSB
#define ATSerial Serial1

// De- und Serialisation
JsonDocument docInput;
JsonDocument docOutput;

// Cell und Batterie
char cell_infos[256];    // Zellinformationen
float batterypercentage; // Batterieprozentsatz

// extern bool gnssIsOn;

// Module
_Battery _BoardBattery;

void setup()
{
  DSerial.begin(115200);
  delay(3000);
  ATSerial.begin(115200);
  delay(3000);

  InitModemMQTT();
  InitGNSS();
}

void loop()
{
  char payload[1028];
  Mqtt_URC_Event_t ret = _AWS.WaitCheckMQTTURCEvent(payload, 2);

  
  switch (ret)
  {
  case MQTT_RECV_DATA_EVENT:
    DSerial.println("RECV_DATA_EVENT");
    handleMQTTEvent(docOutput, payload);
    break;
  case MQTT_STATUS_EVENT:
    handleMQTTStatusEvent(payload);
    break;
  default:
    DSerial.println(ret);
    DSerial.println("Unknow Ret");
    break;
  }

  if (millis() - pub_time < frequenz)
    return;

  pub_time = millis();

  if (GnssMode)
    handleGNSSMode(docInput);
  else if (gnssIsOn)
  {
    _GNSS.TurnOffGNSS();
    gnssIsOn = false;
    timeToFirstFix = 0;
    gnssStartMillis = 0;
  }
  if (TemperatureMode)
  {
    docInput["Temperature"] = 8;
    docInput["Humidity"] = 59;
  }
  if (BatteryMode)
    docInput["BatteryPercentage"] = _BoardBattery.calculateBatteryPercentage();
  if (CellInfosMode)
  {
    _AWS.ReportCellInformation(const_cast<char *>("servingcell"), cell_infos);
    docInput["CellInfos"] = cell_infos;
  }
  docInput["Timestamp"] = _GNSS.GetCurrentTime();
  if (!GnssMode && !CellInfosMode && !BatteryMode && !TemperatureMode)
  {
    docInput["RequestMode"] = true;
    publishData(docInput, payload, pub_time, AT_LEAST_ONCE);
    return;
  }
  publishData(docInput, payload, pub_time, AT_LEAST_ONCE);
}
