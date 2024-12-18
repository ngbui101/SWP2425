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

  initModem(DSerial, _BG96, _ArdruinoZero);
  modemOff = false;
  motionDetected = false;
}

// // Funktion, um nach dem Aufwachen nur die benötigten Komponenten neu zu initialisieren
// void handleWakeUp()
// {
//   // Hier nur diejenigen Initialisierungen vornehmen, die nach dem Schlaf notwendig sind,
//   // z.B. Modem wieder aktivieren, MQTT reconnecten, etc.
//   initModem(DSerial, _BG96, _ArdruinoZero);
//   modemOff = false;
// }

// void goToSleep()
// {
//   // Modem ausschalten, MC in DeepSleep versetzen
//   _BG96.CloseMQTTNetwork(MQTTIndex);
//   _BG96.DeactivateDevAPN(PDPIndex);
//   _BG96.PowerOffModule();
//   modemOff = true;
//   DSerial.println("\nPower Off Module");
//   _ArdruinoZero.deepSleep();
//   // Ab hier schläft der MC, bis ein Wake-Up Interrupt erfolgt
// }

void loop()
{
  char recv_data[1028];
  JsonDocument docInput;
  docInput["IMEI"] = "862315069391414";
  docInput["RequestMode"] = true;
  docInput["Timestamp"] = "2024-12-06T22:24:44";
  char payload[1028];
  serializeJsonPretty(docInput, payload);
  _BG96.HTTPPOST(payload, 80);
  // _BG96.HTTPGET(payload, 80);

  if (_BG96.HTTPRead(recv_data, 80))
  {
    DSerial.println("\r\nHTTP Read Success!");
    DSerial.println("");
    DSerial.println(recv_data);
    DSerial.println("");
  }
  while (1);
}
