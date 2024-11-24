/*
*Die Klasse _BG96_Common erweitert die Funktionalitäten der Basisklasse _BG96_Serial, 
*um erweiterte Funktionen des BG96-Moduls bereitzustellen. Sie bietet Methoden zur Steuerung 
*und Konfiguration des Moduls, wie das Ein- und Ausschalten, das Setzen von 
*Konfigurationsparametern, das Abrufen von Geräteinformationen und Netzstatus sowie 
*das Verwalten von SIM-Karteninformationen.
 */
#ifndef _BG96_COMMEN_H_
#define _BG96_COMMEN_H_

#include "BG96_Serial.h"
#include "Cell.h"

#define POWKEY_PIN 4
#define RESET_PIN 5
#define ENABLE_PWR 2
#define STATUS_PIN 3

typedef enum functionality{
    MINIMUM_FUNCTIONALITY = 0, 
    FULL_FUNCTIONALITY = 1,
    RESET_FUNCTIONALITY = 3,
    DISABLE_RF = 4,
}Functionality_t;

typedef enum cmd_status{
    READ_MODE = 0,
    WRITE_MODE = 1,
}Cmd_Status_t;

typedef enum net_status{
    NOT_REGISTERED = 0,
    REGISTERED = 1,
    SEARCHING = 2,
    REGISTRATION_DENIED = 3,
    UNKNOWN = 4,
    REGISTERED_ROAMING = 5,
}Net_Status_t;

typedef enum net_type{
    GSM = 0,
    LTE_CAT_M1 = 8,
    LTE_CAT_NB1 = 9,
}Net_Type_t;


class _BG96_Common : public _BG96_Serial
{
  public:
    _BG96_Common();

    ~_BG96_Common();

    _BG96_Common(Stream &atserial, Stream &dserial);
    
    bool PowOffModule();

    bool PowOnModule();

    bool TurnOnModule();
	
    bool InitModule();

    bool ResetModule();

    bool SetDevCommandEcho(bool echo);

    bool SetDevOutputformat(bool echo);

    bool GetDevInformation(char *inf);

    bool GetLatestGMTTime(char *time);

    char* GetCurrentTime();

    bool GetDevVersion(char *ver);

    bool GetDevIMEI(char *imei);

    Cmd_Response_t SetDevFunctionality(Functionality_t mode);

    bool DevLocalRate(unsigned long &rate, Cmd_Status_t status);

    bool GetDevSimIMSI(char *imsi);

    bool DevSimPIN(const char *pin, Cmd_Status_t status);

    bool GetDevSimICCID(char *iccid);

    Net_Status_t DevNetRegistrationStatus();

    bool GetDevNetSignalQuality(unsigned int &rssi);

    Cmd_Response_t ScanOperatorNetwork(char *net);

    Cmd_Response_t DevOperatorNetwork(unsigned int &mode, unsigned int &format, const char *&oper, Net_Type_t &act, Cmd_Status_t status);

    bool GetDevNetworkInformation(char *type, char *oper, char *band, char *channel);

    bool DevNetPacketCounter(unsigned long &send_bytes, unsigned long &recv_bytes, bool clean);

    bool DevPowerDown();

    bool DevClock(char *d_clock, Cmd_Status_t status);

    bool ScanmodeConfig(int mode);

    bool SearchingConfig(const char * scanseq);

    time_t parseTimestamp(const char *timestamp);

    Cell* ReportCellInformation(const char *celltype);

    int ReportNeighbourCellInformation(Cell* cells[], int max_cells);
    bool LTENetworkCategoryConfig(int mode);

    bool BandConfig(const char *gsmbandval, const char * catm1bandval, const char * catnb1bandval);

    bool ConfigNetworks();

    bool ResetFunctionality();

    bool ServiceDomainConfig(int service);

    bool NetworkRegistrationCodeConfig(int n);

    int ScanCells(const char* rat, Cell* cells[]);

  private:
    char currenttime[64];
};

#endif