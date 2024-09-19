#include <board.h>

#define DSerial SerialUSB
#define ATSerial Serial1

char APN[] = "wm";
char LOGIN[] = "";
char PASSWORD[] = "";
char tcp_ip[] = "13.237.19.148";
int tcp_port = 3000;
char send_data[256];

unsigned int comm_pdp_index = 1;
unsigned int comm_socket_index = 0;

Socket_Type_t socket = TCP_CLIENT;

_BG96_TCPIP _BG96(ATSerial, DSerial);
_BG96_GNSS _GNSS(ATSerial, DSerial);

unsigned long startTime = 0;
unsigned long endTime = 0;
bool gnssSuccess = false;
unsigned int sendCounter = 0;

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

  if(_BG96.TurnOnModule()){
    _BG96.ResetModule();
    _BG96.SetDevOutputformat(true);
    _BG96.SetDevCommandEcho(false);
    DSerial.println("BG96 Module Initialized");
  }

  char apn_error[64];
  _BG96.InitAPN(comm_pdp_index, APN, "", "", apn_error);

  if (_BG96.OpenSocketService(comm_pdp_index, comm_socket_index, socket, tcp_ip, tcp_port, 0, DIRECT_PUSH_MODE))
  {
    DSerial.println("\r\nOpen Socket Service Success!");
  }
  else
  {
    DSerial.println("\r\nOpen Socket Service Failed!");
    return;
  }

  _BG96.SetDevCommandEcho(true);

  if (_GNSS.TurnOnGNSS(STAND_ALONE, WRITE_MODE))
  {
    DSerial.println("\r\nGNSS is activated.");
    startTime = millis();
  }
  else
  {
    DSerial.println("\r\nFailed to activate GNSS.");
    return;
  }
}

void loop()
{
  char gnss_pos[128];
  if (!gnssSuccess)
  {
    while (ATSerial.read() >= 0)
      ;
    DSerial.println("Trying to get GNSS Position...");
    if (_GNSS.GetGNSSPositionInformation(gnss_pos))
    {
      delay(1000);
      DSerial.println("\r\nGNSS Position:");
      DSerial.println(gnss_pos);

      endTime = millis();
      unsigned long duration = (endTime - startTime) / 1000;
      DSerial.print("Time to get GNSS Fix: ");
      DSerial.print(duration);
      DSerial.println(" seconds");

      sprintf(send_data, "GNSS Position: %s", gnss_pos);
      gnssSuccess = true;
    }
    else
    {
      DSerial.println("\r\nFailed to get GNSS Position. Retrying in 5 seconds...");
      delay(5000);
    }
  }

  if (gnssSuccess && sendCounter < 20)
  {
    if (_BG96.SocketSendData(comm_socket_index, socket, send_data, "", tcp_port))
    {
      DSerial.println("\r\nSocket Send Data Success!");
      sendCounter++;
    }
    else
    {
      DSerial.println("\r\nSocket Send Data Failed!");
    }

    delay(1000);
  }

  if (sendCounter >= 20)
  {
    DSerial.println("\r\n20 successful send attempts reached. Shutting down GNSS and socket...");

    if (_GNSS.TurnOffGNSS())
    {
      DSerial.println("\r\nGNSS turned off.");
    }

    if (_BG96.CloseSocketService(comm_socket_index))
    {
      DSerial.println("\r\nSocket closed.");
    }

    while (1)
      ;
  }
}
