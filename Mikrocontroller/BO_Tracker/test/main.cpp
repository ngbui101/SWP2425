#include <board.h>
#include <MQTT_AWS.hpp>
#include <GNSS.hpp>
#include <Battery.h>
#include <ArduinoJson.h>

#define DSerial SerialUSB
#define ATSerial Serial1

StaticJsonDocument<600> docInput;
StaticJsonDocument<600> docOutput;

char APN[] = "internet.m2mportal.de";
// char APN[] = "iot.1nce.net";
// char APN[] = "wm";
char LOGIN[] = "";
char PASSWORD[] = "";

char mqtt_server[] = "a336z3b6pu6hdu-ats.iot.us-east-1.amazonaws.com";
unsigned int mqtt_port = 8883;
char mqtt_clientId[] = "BG96";
// char mqtt_topicName[50];
char mqtt_sub_topic[64]; // Für "tracker/{IMEI}/sub"
char mqtt_pub_topic[64]; // Für "tracker/{IMEI}/pub"

unsigned int mqtt_index = 0;
Mqtt_Qos_t mqtt_qos = AT_MOST_ONCE;
unsigned long pub_time;
unsigned long gnss_start_time;

// IMEI of the modem
char IMEI[20];
// mode
bool GnssMode = true;

_BG96_MQTT _AWS(ATSerial, DSerial);
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

  _AWS.InitModule();
  _AWS.SetDevCommandEcho(false);
  _AWS.SetDevOutputformat(true);

  // InitModemMQTT(_AWS, DSerial, APN, LOGIN, PASSWORD,
  //               mqtt_server, mqtt_port,
  //               mqtt_clientId, mqtt_sub_topic, mqtt_pub_topic,
  //               AT_MOST_ONCE, mqtt_index,
  //               1, 2, IMEI);

  // InitGNSS(_GNSS, DSerial);
}
void loop()
{
  if (ATSerial.available())
  {
    char at = ATSerial.read();
    DSerial.write(at);
  }
  if (DSerial.available())
  {
    char d = DSerial.read();
    ATSerial.write(d);
    DSerial.write(d);
  }
}