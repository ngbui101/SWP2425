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

// Funktion, um nach dem Aufwachen nur die benötigten Komponenten neu zu initialisieren

void goToSleep()
{
  // Modem ausschalten, MC in DeepSleep versetzen
  _BG96.CloseMQTTNetwork(MQTTIndex);
  _BG96.DeactivateDevAPN(PDPIndex);
  _BG96.PowerOffModule();
  onSleep = true;
  DSerial.println("\nPower Off Module");
  // _ArdruinoZero.deepSleep();
  // Ab hier schläft der MC, bis ein Wake-Up Interrupt erfolgt
}

void loop()
{
  // Prüfe, ob die Frequenz hoch ist (z.B. > 600s)
  if (trackerModes.frequenz > 600000UL)
  { 
    if (onSleep)
    {
      // Versuche Aufwach-Event über Bewegung
      if (!_ArdruinoZero.waitWakeOnMotions())
      {
        return;
      }
      else
      {
        delay(10000); // warte 10s
        if (_ArdruinoZero.stillOnMotions())
        { 
          onSleep = false;
          handleWakeUp(DSerial, _BG96);
        }
        goToSleep();
        return;
      }
    }else{
      if (millis() - pub_time < 600000UL){
        goToSleep();
      }
    }
  }
  else
  {
    if (millis() - pub_time < trackerModes.frequenz)
      return;
  }

  // Auf neue MQTT-Nachrichten prüfen
  waitAndCheck(DSerial, _BG96, docOutput);

  // Modus-abhängige Daten erfassen und versenden
  modeHandle(DSerial, _BG96, docInput, _ArdruinoZero);

  // Tägliches Update prüfen
  // if (millis() - lastUpdateCheck >= UPDATE_INTERVAL)
  // {
  //   lastUpdateCheck = millis();
  //   DailyUpdates(DSerial, _BG96, docInput, _ArdruinoZero, _ArdruinoZero);
  // }
}
