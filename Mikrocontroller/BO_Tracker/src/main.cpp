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
bool modemOff;       // true, wenn Modem ausgeschaltet ist
bool motionDetected; // true, wenn bereits Bewegung erkannt und MC wach ist

void setup()
{
  DSerial.begin(115200);
  while (DSerial.read() >= 0)
    ; // Buffer leeren
  ATSerial.begin(115200);
  while (ATSerial.read() >= 0)
    ; // Buffer leeren
  delay(3000);

  initModul(DSerial, _BG96, _ArdruinoZero);
  modemOff = false;
  motionDetected = false;
}

// Funktion, um nach dem Aufwachen nur die benötigten Komponenten neu zu initialisieren
void handleWakeUp()
{
  // Hier nur diejenigen Initialisierungen vornehmen, die nach dem Schlaf notwendig sind,
  // z.B. Modem wieder aktivieren, MQTT reconnecten, etc.
  initModul(DSerial, _BG96, _ArdruinoZero);
  modemOff = false;
}

void goToSleep()
{
  // Modem ausschalten, MC in DeepSleep versetzen
  _BG96.CloseMQTTNetwork(MQTTIndex);
  _BG96.DeactivateDevAPN(PDPIndex);
  _BG96.PowerOffModule();
  modemOff = true;
  DSerial.println("\nPower Off Module");
  _ArdruinoZero.deepSleep();
  // Ab hier schläft der MC, bis ein Wake-Up Interrupt erfolgt
}

void loop()
{
  // Prüfe, ob die Frequenz hoch ist (z.B. > 600s)
  if (trackerModes.frequenz > 600000UL)
  {
    // Wenn noch keine Bewegung erkannt wurde:
    if (!motionDetected)
    {
      // Versuche Aufwach-Event über Bewegung
      if (!_ArdruinoZero.waitWakeOnMotions())
      {
        // Keine Bewegung erkannt -> schlafe ein
        goToSleep();
        // Nach deepSleep wartet der MC auf Interrupt. loop() pausiert hier.
        return;
      }
      else
      {
        // Bewegung erkannt, MC ist nun wach

        motionDetected = true;
        if (modemOff)
        {
          handleWakeUp();
        }
      }
    }
  }

  // Wenn wir nicht schlafen, führen wir reguläre Aktionen aus
  // Warteintervall abprüfen
  if (millis() - pub_time < 10000UL)
  {
    return;
  }

  // Auf neue MQTT-Nachrichten prüfen
  waitAndCheck(DSerial, _BG96, docOutput);

  // Modus-abhängige Daten erfassen und versenden
  modeHandle(DSerial, _BG96, docInput, _ArdruinoZero);

  // Tägliches Update prüfen
  if (millis() - lastUpdateCheck >= UPDATE_INTERVAL)
  {
    lastUpdateCheck = millis();
    DailyUpdates(DSerial, _BG96, docInput, _ArdruinoZero, _ArdruinoZero);
  }

  // Option: Nach erfolgreichem Senden erneut prüfen, ob der MC wieder schlafen soll
  // Wenn Frequenz weiterhin hoch ist und wir bereits wach waren, können wir erneut schlafen
  if (trackerModes.frequenz > 600000UL && motionDetected && !_ArdruinoZero.stillOnMotions())
  {
    motionDetected = false; // Zurücksetzen, um bei nächster Gelegenheit wieder auf Bewegung zu warten
    // Modem kann nun wieder ausgeschaltet werden, MC schläft erneut ein
    goToSleep();
  }
}
