#include "Arduino.h"
#include "BG96_GNSS.h"

#define DSerial SerialUSB
#define ATSerial Serial1
#define UART_DEBUG

GNSS_Work_Mode_t mode = STAND_ALONE;
_BG96_GNSS _5GNBIoT(ATSerial, DSerial);

void setup()
{
  DSerial.begin(115200);
  while (DSerial.read() >= 0)
    ;
  DSerial.println("This is the _5GNBIoT Debug Serial!");
  ATSerial.begin(115200);
  while (ATSerial.read() >= 0)
    ;
  delay(1000);
  if (_5GNBIoT.InitModule())
  {
    DSerial.println("\r\n_5GNBIoT.InitModule() OK!");
  }
  delay(1000);
  // if(_5GNBIoT.SetGNSSOutputPort(UARTNMEA)){
  //   DSerial.println("\r\nSet GNSSOutputPort OK!");
  // }
  _5GNBIoT.SetDevCommandEcho(false);
  delay(100);
  if (_5GNBIoT.TurnOnGNSS(mode, WRITE_MODE))
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
  if (_5GNBIoT.GetGNSSPositionInformation(gnss_posi))
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