#include <MQTT_AWS.hpp>
#include <GNSS.hpp>
#include <ArduinoJson.h>
#include <Temperature.h>

#define DSerial SerialUSB
#define ATSerial Serial1

// De- und Serialisation
JsonDocument docInput;
JsonDocument docOutput;

// Cell und Batterie
float batterypercentage;

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

void loop()
{
  if (millis() - pub_time < trackerModes.frequenz)
    return;
  if (trackerModes.Modem_Off)
  {
    InitModemMQTT();
    trackerModes.Modem_Off = false;
  }
  else
  {
    waitAndCheck(docOutput);
    modeHandle(docInput, _BoardBattery);
  }
  if (millis() - lastUpdateCheck >= UPDATE_INTERVAL)
  {
    lastUpdateCheck = millis();
    DailyUpdates(docInput, _BoardBattery);
  }
}
