#include <Mode_Handle.hpp>

#define DSerial SerialUSB
#define ATSerial Serial1

// De- und Serialisation
JsonDocument docInput;
JsonDocument docOutput;

// Zeitintervall für das tägliche Update (24 Stunden in Millisekunden)
const unsigned long UPDATE_INTERVAL = 86400000UL;
unsigned long lastUpdateCheck = 0;

_Board _ArdruinoZero;

_BG96_Module _BG96(ATSerial,DSerial);

void setup()
{
  DSerial.begin(115200);
  while (DSerial.read() >= 0)
    ;
  ATSerial.begin(115200);
  while (ATSerial.read() >= 0)
    ;
  delay(3000);
  
  initModul(DSerial,_BG96,_ArdruinoZero);
  
}

void loop()
{
  if (millis() - pub_time < trackerModes.frequenz)
    return;
  if (trackerModes.Modem_Off)
  {
    InitModemMQTT(DSerial, _BG96);
    trackerModes.Modem_Off = false;
  }
  else
  {
    waitAndCheck(DSerial, _BG96, docOutput);
    modeHandle(DSerial, _BG96, docInput, _ArdruinoZero);
  }
  if (millis() - lastUpdateCheck >= UPDATE_INTERVAL)
  {
    lastUpdateCheck = millis();
    DailyUpdates(DSerial, _BG96, docInput, _ArdruinoZero);
  }
}
