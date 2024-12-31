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

// char payload[] = R"({
//     "token": "pk.11e2626f93a4d4ee0bb5ab37ee6b25a4",
//     "mcc": 262,
//     "cells": [
//         {"radio": "lte", "mnc": 01, "lac": 13490, "cid": 27105040, "signal": -88},
//         {"radio": "lte", "mnc": 02, "lac": 45139, "cid": 13435906, "signal": -112},
//         {"radio": "lte", "mnc": 03, "lac": 35050, "cid": 13969153, "signal": -111}
//     ],
//     "address": 1
// })";

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
  char header[1028];
  char fullRequest[2056];
  char payload[1024];
  snprintf(header, sizeof(header),
        "GET / HTTP/1.0\r\n\r\n"
        "Host: https://xsq63wtz3xaoay5zkbxofnlymq0dpgag.lambda-url.eu-central-1.on.aws/\r\n\r\n"
        ); // Content-Length basierend auf Payload

    // Beispiel-Payload
    snprintf(payload, sizeof(payload),
        "{\n"
        "  \"IMEI\": \"862315069391414\",\n"
        "  \"RequestMode\": true,\n"
        "  \"Timestamp\": \"2024-12-06T22:24:44\"\n"
        "}");

    // Header und Payload zusammenfügen
    snprintf(fullRequest, sizeof(fullRequest), "%s%s", header, payload);

    // Ausgabe des kombinierten Requests
    // printf("Full HTTP Request:\n%s", fullRequest);
  // serializeJsonPretty(docInput, payload);
  // _BG96.HTTPPOST(fullRequest, 80);
  _BG96.HTTPGET(fullRequest, 80);
  // _BG96.HTTPGET(80);

  if (_BG96.HTTPRead(recv_data, 80))
  {
    DSerial.println("\r\nHTTP Read Success!");
    DSerial.println("");
    DSerial.println(recv_data);
    DSerial.println("");
  }
  while (1);
}
