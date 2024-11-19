#include <MQTT_AWS.hpp>
#include <GNSS.hpp>
#include <Battery.h>
#include <ArduinoJson.h>
#include <Temperature.h>

#define DSerial SerialUSB
#define ATSerial Serial1

// De- und Serialisation
JsonDocument docInput;
JsonDocument docOutput;

// Cell und Batterie
char cell_infos[256];  
float batterypercentage; 
float humidity;
float temperature;

// Zeitintervall für das tägliche Update (24 Stunden in Millisekunden)
const unsigned long UPDATE_INTERVAL = 86400000UL;
unsigned long lastUpdateCheck = 0;

// Module
_Battery _BoardBattery;
_Temperature _TInstance;

void setup()
{
  DSerial.begin(115200);
  delay(3000);
  ATSerial.begin(115200);
  delay(3000);

  InitModemMQTT();
  InitGNSS();
}

// Funktion für tägliche Updates
void DailyUpdates()
{
  GPSOneXtraCheckForUpdate();
  /*
  if we want to do the same for low/high temperature or humidity:
  humidity = _TInstance().readHumidity();
  temperature = _TInstance().readTemperature();
  if abfragen  
  */
  batterypercentage = _BoardBattery.calculateBatteryPercentage();
  if (batterypercentage <= 10)
  {
    docInput["BatteryLow"] = true;
  }
  if (publishData(docInput, pub_time, AT_LEAST_ONCE, "/notifications"))
  {
    delay(300);
  }
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

  // Tägliches Update ausführen
  if (millis() - lastUpdateCheck >= UPDATE_INTERVAL)
  {
    lastUpdateCheck = millis();
    DailyUpdates();
    return;
  }
  
  // Daten nur in festgelegten Intervallen veröffentlichen
  if (millis() - pub_time < trackerModes.frequenz)
    return;

  pub_time = millis();

  // GNSS-Modus verwalten
  if (trackerModes.GnssMode)
  {
    handleGNSSMode(docInput);
  }
  else if (gnssTracker.isOn)
  {
    _GNSS.TurnOffGNSS();
    gnssTracker.isOn = false;
    gnssTracker.timeToFirstFix = 0;
    gnssTracker.startMillis = 0;
  }

  // Temperatur- und Feuchtigkeitsdaten sammeln
  if (trackerModes.TemperatureMode)
  {
    docInput["Temperature"] = _TInstance.readTemperature();
    docInput["Humidity"] = _TInstance.readHumidity();
  }

  // Batteriestand erfassen
  if (trackerModes.BatteryMode)
  {
    batterypercentage = _BoardBattery.calculateBatteryPercentage();
    docInput["BatteryPercentage"] = batterypercentage;
  }

  // Zellinformationen erfassen
  if (trackerModes.CellInfosMode)
  {
    _AWS.ReportCellInformation(const_cast<char *>("servingcell"), cell_infos);
    docInput["CellInfos"] = cell_infos;
  }

  // Request-Modus setzen
  trackerModes.updateRequestMode();
  if (trackerModes.RequestMode)
  {
    docInput["RequestMode"] = true;
  }
  // Zeitstempel hinzufügen
  docInput["Timestamp"] = _GNSS.GetCurrentTime();
  // Daten veröffentlichen
  if (publishData(docInput, pub_time, AT_LEAST_ONCE, "/pub"))
  {
    delay(300);
  }
  if (trackerModes.GeoFenMode)
  {
    if (!gnssTracker.geoFenceInit)
      addGeo();
    GEOFENCE_STATUS_t status = _GNSS.getGeoFencingStatus(gnssTracker.geoID);
    switch (status)
    {
    case OUTSIDE_GEOFENCE:
      docInput["LeavingGeo"] = true;
      if (publishData(docInput, pub_time, AT_LEAST_ONCE, "/notifications"))
      {
        delay(300);
      }
      break;
    case INSIDE_GEOFENCE || NOFIX:
      break;
    default:
      break;
    }
  }
}
