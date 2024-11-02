#include <MQTT_AWS.hpp>
#include <GNSS.hpp>
#include <Battery.h>
#include <ArduinoJson.h>

#define DSerial SerialUSB
#define ATSerial Serial1

// De- und Serialisation
DynamicJsonDocument docInput(600);

extern unsigned int MQTTIndex;
// Modi
extern bool GnssMode;
extern bool CellInfosMode;
extern bool BatteryMode;
extern bool NmeaMode; 
extern unsigned int frequenz;

// GNSS und Batterie
char IMEI[20];
char gnss_posi[128];     // GNSS-Position
char gnss_gsa[128];      // GSA-NMEA-Satz
char gnss_gsv[516];      // GSV-NMEA-Satz
char cell_infos[256];    // Zellinformationen
float batterypercentage; // Batterieprozentsatz
float accuracy = 0.0;    // Genauigkeit

unsigned long gnssStartMillis = 0;
unsigned long timeToFirstFix = 0;
GNSS_Work_Mode_t mode = MS_BASED;
bool gnssIsOn = false;

// Module
extern _BG96_MQTT _AWS; 
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

  // Initialisierung des Modems und GNSS
  // _AWS.InitModule();
  // _AWS.SetDevCommandEcho(false);
  // _AWS.SetDevOutputformat(true);

  // Anpassungen für die Konvertierung auf `const` angepasst
  InitModemMQTT();
  InitGNSS(_GNSS, DSerial);
}

void handleGNSSMode()
{
  if (!gnssIsOn && _GNSS.TurnOnGNSS(mode, WRITE_MODE))
  {
    gnssIsOn = true;
    gnssStartMillis = millis();
  }

  if (_GNSS.GetGNSSPositionInformation(gnss_posi) && _GNSS.GetEstimationError(accuracy))
  {
    timeToFirstFix = timeToFirstFix ? timeToFirstFix : millis() - gnssStartMillis;
    docInput["TimeToGetFirstFix"] = timeToFirstFix;
    docInput["Position"] = gnss_posi;
    docInput["Accuracy"] = accuracy;
  }

  if (NmeaMode)
  {
    _GNSS.GetGNSSNMEASentences(GPGSA, gnss_gsa);
    docInput["GSA"] = gnss_gsa;
    _GNSS.GetGNSSNMEASentences(GPGSV, gnss_gsv);
    docInput["GSV"] = gnss_gsv;
  }
}

void loop()
{ 
  char payload[1028];
  Mqtt_URC_Event_t ret = _AWS.WaitCheckMQTTURCEvent(payload, 2);

  switch (ret)
  {
  case MQTT_RECV_DATA_EVENT:
    handleMQTTEvent(payload); // Aufruf der ausgelagerten Funktion
    break;
  case MQTT_STATUS_EVENT:
    handleMQTTStatusEvent(payload); // Status-Event-Handling
    break;
  default:
    break;
  }
  if (millis() - pub_time < frequenz)
    return;

  pub_time = millis();
  if (GnssMode)
    handleGNSSMode();
  else if (gnssIsOn)
  {
    _GNSS.TurnOffGNSS();
    gnssIsOn = false;
  }
  if (BatteryMode)
    docInput["BatteryPercentage"] = _BoardBattery.calculateBatteryPercentage();
  if (CellInfosMode){
    _AWS.ReportCellInformation(const_cast<char *>("servingcell"), cell_infos);
    docInput["CellInfos"] = cell_infos;
  }
  docInput["Timestamp"] = _GNSS.GetCurrentTime();
  publishData(docInput, payload, pub_time,AT_LEAST_ONCE);
}
