#include <Mode_Handle.hpp>

#define DSerial SerialUSB
#define ATSerial Serial1

// De- und Serialisation
JsonDocument docInput;
JsonDocument docOutput;

// Zeitintervall für tägliche Updates (24 Stunden in ms)
const unsigned long UPDATE_INTERVAL = 86400000UL;
unsigned long lastUpdateCheck = 0;

_Board _ArdruinoZero;
_BG96_Module _BG96(ATSerial, DSerial);

// Status-Flags
bool onSleep = false;

void setup()
{
  DSerial.begin(115200);
  while (DSerial.read() >= 0)
    ; // Buffer leeren
  delay(3000);
  ATSerial.begin(115200);
  while (ATSerial.read() >= 0)
    ; // Buffer leeren
  delay(3000);
  initModul(DSerial, _BG96, _ArdruinoZero);
}


void goToSleep(int millis)
{
  _BG96.CloseMQTTNetwork(MQTTIndex);
  _BG96.DeactivateDevAPN(PDPIndex);
  _BG96.PowerOffModule();
  onSleep = true;
  DSerial.println("\nPower Off Module");
  _ArdruinoZero.deepSleep(millis);
}

void loop()
{


  if (onSleep)
  {
    // Versuche Aufwach-Event über Bewegung
    if (!_ArdruinoZero.waitWakeOnMotions())
    {
      if (millis() - pub_time > trackerModes.period)
      {
        DSerial.println("Wake Up.....");
        onSleep = false;
        handleWakeUp(DSerial, _BG96);
      }
      else
        return;
    }
    else
    {
      if (!_ArdruinoZero.checkOnMotionsfor10s())
      {
        goToSleep(120000);
        return;
      }
      DSerial.println("Wake Up.....");
      onSleep = false;
      handleWakeUp(DSerial, _BG96);
    }
  }
  else
  {
    if (trackerModes.period <= 600000)
    {
      if (millis() - pub_time < trackerModes.period)
      {
        return;
      }
    }
  }

  // Auf neue MQTT-Nachrichten prüfen
  waitAndCheck(DSerial, _BG96, docOutput);

  // Modus-abhängige Daten erfassen und versenden
  modeHandle(DSerial, _BG96, docInput, _ArdruinoZero);

  if (trackerModes.period > 600000 && !_ArdruinoZero.checkOnMotionsfor10s())
    goToSleep(120000);
  // Tägliches Update prüfen
  // if (millis() - lastUpdateCheck >= UPDATE_INTERVAL)
  // {
  //   lastUpdateCheck = millis();
  //   DailyUpdates(DSerial, _BG96, docInput, _ArdruinoZero, _ArdruinoZero);
  // }
}
