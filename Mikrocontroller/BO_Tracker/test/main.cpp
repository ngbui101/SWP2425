#include <BG96_Common.h>
#define DSerial SerialUSB
#define ATSerial Serial1
_BG96_Common _BG96(ATSerial, DSerial);
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
    _BG96.InitModule();
    DSerial.println("\r\n_BG96.InitModule() OK!");
    _BG96.SetDevCommandEcho(false);
    // ATSerial.println("AT+QGPS=1");
    // delay(1000);
}
void loop()
{   
    // ATSerial.println("AT+QGPSLOC?");
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
