#include <board.h>
#define DSerial SerialUSB
#define ATSerial Serial1
char APN[] = "wm";
unsigned int comm_pdp_index = 1; // The range is 1 ~ 16
_BG96_TCPIP _BG96(ATSerial, DSerial);
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
    char apn_error[64];
    _BG96.InitAPN(comm_pdp_index, APN, "", "", apn_error);
}
void loop()
{
}
