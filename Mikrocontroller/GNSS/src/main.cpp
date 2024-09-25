#include <board.h>
#include <BG96_AWS.hpp>
#include <ArduinoJson.h>

#define DSerial SerialUSB
#define ATSerial Serial1

StaticJsonDocument<600> docInput;
StaticJsonDocument<600> docOutput;

char APN[] = "wm";
char LOGIN[] = "";
char PASSWORD[] = "";

char mqtt_server[] = "a336z3b6pu6hdu-ats.iot.us-east-1.amazonaws.com";
unsigned int mqtt_port = 8883;
char mqtt_clientId[] = "BG96";
char mqtt_topicName[] = "MyTopic";
unsigned int mqtt_index = 3;
Mqtt_Qos_t mqtt_qos = AT_MOST_ONCE;
unsigned long pub_time;

// send
char send_data[256];
String inputBuffer = "";
// IMEI of the modem
char IMEI[20];

_BG96_MQTT _AWS(ATSerial, DSerial);

GNSS_Work_Mode_t mode = STAND_ALONE;
_BG96_GNSS _GNSS(ATSerial, DSerial);

unsigned long startTime = millis();
unsigned long endTime = 0;
bool gnssSuccess = false;
unsigned int sendCounter = 0;
enum ProgramState
{
  STATE_SELECT_MODE,
  STATE_SERIAL_MODE,
  STATE_GNSS_MODE
};
ProgramState currentState = STATE_SELECT_MODE;

void processGNSSMode();
void processSerialMode();
void selectMode();

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

  _AWS.TurnOnModule();
  _AWS.SetDevCommandEcho(false);
  _AWS.SetDevOutputformat(true);

  InitModemMQTT(_AWS, DSerial, APN, LOGIN, PASSWORD,
                mqtt_server, mqtt_port,
                mqtt_clientId, mqtt_topicName,
                AT_MOST_ONCE, mqtt_index,
                1, 2, IMEI);
  
  if(_GNSS.InitGpsOneXTRA()){
    DSerial.println("\r\nInit GpsOneXTRA Success!");
  }

  while (!_GNSS.TurnOnGNSS(mode, WRITE_MODE))
  {
    DSerial.println("\r\nOpen the GNSS Function Fali!");
    if (_GNSS.TurnOnGNSS(mode, READ_MODE))
    {
      DSerial.println("\r\nThe GNSS Function is Opened!");
      _GNSS.TurnOffGNSS();
    }
  }
  DSerial.println("\r\nOpen the GNSS Function Success!");
}

void loop()
{
  switch (currentState)
  {
  case STATE_SELECT_MODE:
    selectMode();
    break;

  case STATE_SERIAL_MODE:
    processSerialMode();
    break;

  case STATE_GNSS_MODE:
    processGNSSMode();
    break;
  }
}

///////// END ///////////
void selectMode()
{
  DSerial.println("Bitte wählen Sie den Modus:");
  DSerial.println("1: Serial Mode (AT Commands)");
  DSerial.println("2: GNSS Mode");
  DSerial.print("> "); // Eingabeaufforderung

  while (true)
  {
    if (DSerial.available())
    {
      char choice = DSerial.read();
      DSerial.println(choice); // Eingegebene Wahl anzeigen

      if (choice == '1')
      {
        currentState = STATE_SERIAL_MODE;
        DSerial.println("Serial Mode ausgewählt.");
        break;
      }
      else if (choice == '2')
      {
        currentState = STATE_GNSS_MODE;
        DSerial.println("GNSS Mode ausgewählt.");
        break;
      }
      else
      {
        DSerial.println("Ungültige Auswahl. Bitte '1' oder '2' eingeben.");
        DSerial.print("> ");
      }
    }
  }
}
void processGNSSMode(){
   char payload[256];
  char *sta_buf;
  int res;
  DeserializationError error;
  char gnss_posi[128];

  if (!_GNSS.GetGNSSPositionInformation(gnss_posi)) 
  {
    DSerial.println("\r\nGet the GNSS Position Fail!");
    strcpy(gnss_posi, "no fix");
  }
  else 
  {
    DSerial.println("\r\nGet the GNSS Position Success!");
    DSerial.println(gnss_posi);
  }

 Mqtt_URC_Event_t ret = _AWS.WaitCheckMQTTURCEvent(payload, 2);
  switch (ret)
  {
    case MQTT_RECV_DATA_EVENT:
      error = deserializeJson(docOutput, payload);

      if (error == DeserializationError::Ok)
      {
        if (docOutput["Device"] == "GPS")
        {
          DSerial.println("Device is a Sound sensor!");

          DSerial.println(docOutput["DeviceID"].as<String>());
          DSerial.println(docOutput["Timestamp"].as<double>(), 6);
          DSerial.println(docOutput["Device"].as<String>());
          DSerial.println(docOutput["OpCode"].as<String>());
          DSerial.println(docOutput["Position"].as<String>());
        }
        else
        {
          DSerial.println("Device is not a GPS!");
        }
      }
      else
      {
        DSerial.println("\r\n Error in  Deserialization!");
        DSerial.println(error.c_str());
      }      

      break;

    case MQTT_STATUS_EVENT:
      sta_buf = strchr(payload, ',');
      if (atoi(sta_buf + 1) == 1)
      {
        if (_AWS.CloseMQTTClient(mqtt_index))
        {
          DSerial.println("\r\nClose the MQTT Client Success!");
        }
      }
      else
      {
        DSerial.print("\r\nStatus cade is :");
        DSerial.println(atoi(sta_buf + 1));
        DSerial.println("Please check the documentation for error details.");
      }
      break;
  }
  
  if (millis() - pub_time >= 5000UL) 
  {
    pub_time = millis();

    // ...... Previous code lines go here

    docInput["DeviceID"] = IMEI;
    docInput["Timestamp"] = millis();
    docInput["Device"] = "GPS";
    docInput["OpCode"] = "Read";
    docInput["Position"] = gnss_posi;
    serializeJsonPretty(docInput, payload);

    // ..... Next code lines go here
        
    res = _AWS.MQTTPublishMessages(mqtt_index, 1,
                                       AT_LEAST_ONCE,
                                       mqtt_topicName,
                                       false,
                                       payload);

    if ((res == PACKET_SEND_SUCCESS_AND_RECV_ACK)
        || (res == PACKET_RETRANSMISSION))
    {
      DSerial.println("Publish Success!");
    }
    else
    {
      DSerial.println("Publish failed!");
    }
  }

  delay(1000);
}
void processSerialMode()
{
  while (DSerial.available())
  {
    char d = DSerial.read();
    ATSerial.write(d);
    DSerial.write(d);

    if (d == '\n' || d == '\r')
    {
      inputBuffer.trim();
      if (inputBuffer.equalsIgnoreCase("GNSS"))
      {
        DSerial.println("\nWechsle in den GNSS Mode...");
        currentState = STATE_GNSS_MODE;
        inputBuffer = "";
        return;
      }
      else if (inputBuffer.equalsIgnoreCase("EXIT"))
      {
        DSerial.println("\nBeende das Programm.");
        while (true)
          ;
      }
      else
      {
        inputBuffer = "";
      }
    }
    else
    {
      inputBuffer += d;
    }
  }

  while (ATSerial.available())
  {
    char at = ATSerial.read();
    DSerial.write(at);
  }
}