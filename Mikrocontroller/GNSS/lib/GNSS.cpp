#include "Arduino.h"
#include "BG96_GNSS.h"

#define DSerial SerialUSB
#define ATSerial Serial1
#define UART_DEBUG

GNSS_Work_Mode_t mode = STAND_ALONE;
_BG96_GNSS _BG96(ATSerial, DSerial);

void setup()
{
  DSerial.begin(115200);
  while (DSerial.read() >= 0)
    ;
  DSerial.println("This is the _BG96 Debug Serial!");
  ATSerial.begin(115200);
  while (ATSerial.read() >= 0)
    ;
  delay(1000);
  if (_BG96.InitModule())
  {
    DSerial.println("\r\n_BG96.InitModule() OK!");
  }
  delay(1000);
  // if(_BG96.SetGNSSOutputPort(UARTNMEA)){
  //   DSerial.println("\r\nSet GNSSOutputPort OK!");
  // }
  _BG96.SetDevCommandEcho(false);
  delay(100);
  if (_BG96.TurnOnGNSS(mode, WRITE_MODE))
  {
    DSerial.println("\r\nOpen the GNSS Function Success!");
  }
  else
  {
    DSerial.println("\r\nFail to open GNSS Function!");
  }
}
void loop()
{
  char gnss_posi[128];
  if (_BG96.GetGNSSPositionInformation(gnss_posi))
  {
    DSerial.println("\r\nGet the GNSS Position Success!");
    DSerial.println(gnss_posi);
  }
  else
  {
    DSerial.println("\r\nFail to get the GNSS Position!");
  }
  delay(3000);
}