#include <board.h>
#define DSerial SerialUSB
#define ATSerial Serial1
char APN[] = "wm";
unsigned int comm_pdp_index = 1; // The range is 1 ~ 16
_BG96_TCPIP _5GNBIoT(ATSerial, DSerial);
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
    char apn_error[64];
    _5GNBIoT.InitAPN(comm_pdp_index, APN, "", "", apn_error);
}
void loop()
{
}
