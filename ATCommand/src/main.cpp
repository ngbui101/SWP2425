#include <board.h>
#define DSerial SerialUSB
#define ATSerial Serial1
_BG96_Common _5GNBIoT(ATSerial, DSerial);
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
    _5GNBIoT.InitModule();
    DSerial.println("\r\n_5GNBIoT.InitModule() OK!");
    _5GNBIoT.SetDevCommandEcho(false);
    ATSerial.println("AT+QGPS=1");
    delay(1000);
}
void loop()
{   
    ATSerial.println("AT+QGPSLOC?");
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
