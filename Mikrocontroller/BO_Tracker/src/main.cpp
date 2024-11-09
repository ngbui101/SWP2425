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

//Zeitintervall für Update
const unsigned long UPDATE_INTERVAL = 86400000UL; // 24 Stunden in Millisekunden
unsigned long lastUpdateCheck = 0;

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
void DailyUpdates()
{
  GPSOneXtraCheckForUpdate();
  batterypercentage = _BoardBattery.calculateBatteryPercentage();
  if(batterypercentage <= 10){
    docInput["BatteryLow"] = true;
  }
  if(publishData(docInput, pub_time, AT_LEAST_ONCE,"/notifications")){
    delay(300);
    return;
  };
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
    break;
  }
  if (millis() - lastUpdateCheck >= UPDATE_INTERVAL) {
    lastUpdateCheck = millis();  
    DailyUpdates();
    return;          
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
    batterypercentage = _BoardBattery.calculateBatteryPercentage();
    docInput["BatteryPercentage"] = batterypercentage;
  if (CellInfosMode)
  {
    _AWS.ReportCellInformation(const_cast<char *>("servingcell"), cell_infos);
    docInput["CellInfos"] = cell_infos;
  }
  if (ReqestMode)
  {
    docInput["RequestMode"] = true;
  }
  docInput["Timestamp"] = _GNSS.GetCurrentTime();
  if(publishData(docInput, pub_time, AT_LEAST_ONCE,"/pub")){
    delay(300);
    return;
  };
}
