#include <board.h>

#define DSerial SerialUSB
#define ATSerial Serial1

char APN[] = "wm";
char LOGIN[] = "";
char PASSWORD[] = "";
char tcp_ip[] = "13.237.19.148";
int tcp_port = 3000;
char send_data[256];
String inputBuffer = "";

unsigned int comm_pdp_index = 1;
unsigned int comm_socket_index = 0;

Socket_Type_t socket = TCP_CLIENT;

// Nur ein Objekt von Klasse _BG96_GNSS, jetzt umbenannt in _BG96
_BG96_GNSS _BG96(ATSerial, DSerial);

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
void initializeGNSSMode();
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

  // Initialisiere das Modul über _BG96 (früher _GNSS)
  if (_BG96.TurnOnModule())
  {
    _BG96.SetDevOutputformat(true);
    _BG96.SetDevCommandEcho(false);
    DSerial.println("BG96 Module Initialized");
  }

  char apn_error[64];
  _BG96.InitAPN(comm_pdp_index, APN, "", "", apn_error);
  DSerial.println(apn_error);
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
        initializeGNSSMode();
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

void initializeGNSSMode()
{
  // Nutze _BG96, um Socket Service zu öffnen
  if (_BG96.OpenSocketService(comm_pdp_index, comm_socket_index, socket, tcp_ip, tcp_port, 0, BUFFER_MODE))
  {
    DSerial.println("\r\nOpen Socket Service Success!");
  }
  else
  {
    DSerial.println("\r\nOpen Socket Service Failed!");
    return;
  }

  if (_BG96.EnableGpsOneXTRA())
  {
    DSerial.println("\r\nEnable GPSOneXtra.");
  }

  if (_BG96.InitGpsOneXTRA())
  {
    DSerial.println("\r\nGNSS with GpsOneXTRA Assistance is activated.");
    startTime = millis();
  }
  else
  {
    DSerial.println("\r\nFailed to activate GNSS.");
    return;
  }
}

void processGNSSMode()
{
  char gnss_pos[128];

  // GNSS-Position abrufen
  if (!gnssSuccess)
  {
    DSerial.println("Trying to get GNSS Position...");
    if (_BG96.GetGNSSPositionInformation(gnss_pos))
    {
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
      DSerial.println("\r\nFailed to get GNSS Position. Retrying in 5 seconds. Press 'q+Enter' to quit.");

      unsigned long waitStart = millis();
      while (millis() - waitStart < 5000)
      {
        if (DSerial.available())
        {
          char d = DSerial.read();
          if (d == 'q' || d == 'Q')
          {
            DSerial.println("\nZurück zum Auswahlmenü...");
            if (_BG96.TurnOffGNSS())
            {
              DSerial.println("GNSS turned off.");
            }
            delay(300);
            if (_BG96.CloseSocketService(comm_socket_index))
            {
              DSerial.println("Socket closed.");
            }
            delay(300);
            gnssSuccess = false;
            sendCounter = 0;
            currentState = STATE_SELECT_MODE;
            return;
          }
        }
        delay(10);
      }
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

    if (_BG96.TurnOffGNSS())
    {
      DSerial.println("\r\nGNSS turned off.");
    }

    if (_BG96.CloseSocketService(comm_socket_index))
    {
      DSerial.println("\r\nSocket closed.");
    }
    currentState = STATE_SELECT_MODE;
    sendCounter = 0;
    gnssSuccess = false;
    DSerial.println("Wechsle zurück in den Serial Mode.");
  }
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
        initializeGNSSMode();
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
