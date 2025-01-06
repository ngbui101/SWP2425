#include "BG96_TCPIP.h"

/**
 * @class _BG96_TCPIP
 * @brief Klasse zur Verwaltung der TCP/IP-Funktionalität des BG96-Moduls.
 */
_BG96_TCPIP::_BG96_TCPIP()
{
    // Konstruktor ohne Parameter
}

_BG96_TCPIP::~_BG96_TCPIP()
{
    // Destruktor
}

/**
 * @brief Konstruktor mit seriellen Schnittstellen.
 * 
 * @param atserial Stream für AT-Befehle
 * @param dserial Stream für Debug-Ausgaben
 */
_BG96_TCPIP::_BG96_TCPIP(Stream &atserial, Stream &dserial) : _BG96_Common(atserial, dserial)
{
    // Konstruktor mit seriellen Schnittstelleninitialisierung
}

/**
 * @brief Setzt die APN-Parameter für das Gerät.
 * 
 * @param pdp_index Index des PDP-Kontexts
 * @param type Protokolltyp (z.B. IPV4)
 * @param apn Zugangspunktname
 * @param usr Benutzername für die APN
 * @param pwd Passwort für die APN
 * @param met Authentifizierungsmethode
 * @return true bei erfolgreichem Setzen der Parameter, sonst false
 */
bool _BG96_TCPIP::SetDevAPNParameters(unsigned int pdp_index, Protocol_Type_t type, const char *apn, const char *usr, const char *pwd, Authentication_Methods_t met)
{
    char cmd[64], buf[64];
    strcpy(cmd, APN_PARAMETERS);
    sprintf(buf, "=%d,%d,\"%s\",\"%s\",\"%s\",%d", pdp_index, type, apn, usr, pwd, met);
    strcat(cmd, buf);
    
    if (sendAndSearch(cmd, RESPONSE_OK, 2))
    {
        return true;
    }
    return false;
}

/**
 * @brief Aktiviert den APN für das angegebene PDP-Index.
 * 
 * @param pdp_index Index des PDP-Kontexts
 * @return Cmd_Response_t Antwortstatus des Befehls
 */
Cmd_Response_t _BG96_TCPIP::ActivateDevAPN(unsigned int pdp_index)
{
    char cmd[16], buf[8];
    strcpy(cmd, ACTIVATE_APN);
    sprintf(buf, "=%d", pdp_index);
    strcat(cmd, buf);
    return sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 150);
}

/**
 * @brief Deaktiviert den APN für das angegebene PDP-Index.
 * 
 * @param pdp_index Index des PDP-Kontexts
 * @return true bei erfolgreicher Deaktivierung, sonst false
 */
bool _BG96_TCPIP::DeactivateDevAPN(unsigned int pdp_index)
{
    char cmd[16], buf[8];
    strcpy(cmd, DEACTIVATE_APN);
    sprintf(buf, "=%d", pdp_index);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 40) > 0){
        return true;
    }
    return false;
}

/**
 * @brief Ruft die IP-Adresse des APN für das angegebene PDP-Index ab.
 * 
 * @param pdp_index Index des PDP-Kontexts
 * @param ip Puffer zur Speicherung der IP-Adresse
 * @return true bei erfolgreichem Abruf, sonst false
 */
bool _BG96_TCPIP::GetDevAPNIPAddress(unsigned int pdp_index, char *ip)
{
    char cmd[16], buf[8];
    strcpy(cmd, GET_APN_IP_ADDRESS);
    sprintf(buf, "=%d", pdp_index);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, 2)){
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        char *sta_buf = searchChrBuffer(',');
        if (sta_buf) 
        {
            strcpy(ip, sta_buf + 1);
        }
        else // Komma nicht gefunden
        {
            sta_buf = searchChrBuffer('"');
            if (sta_buf) 
            {
                strcpy(ip, sta_buf + 2);
            }
            else // Doppeltes Anführungszeichen nicht gefunden
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

/**
 * @brief Initialisiert die APN mit den angegebenen Parametern.
 * 
 * @param pdp_index Index des PDP-Kontexts
 * @param apn Zugangspunktname
 * @param usr Benutzername für die APN
 * @param pwd Passwort für die APN
 * @param err_code Puffer zur Speicherung von Fehlermeldungen
 * @return true bei erfolgreicher Initialisierung, sonst false
 */
bool _BG96_TCPIP::InitAPN(unsigned int pdp_index, const char* apn, const char* usr, const char* pwd, char* err_code)
{
    // Statusvariablen
    Net_Status_t i_status = NOT_REGISTERED;
    Cmd_Response_t init_status;
    const char *e_str;
    char i_ip[16];  // Buffer für die IP-Adresse
    // Schritt 1: SIM-Kartenprüfung
    unsigned long start_time = millis();
    while (!DevSimPIN("", READ_MODE))
    {
        if (millis() - start_time >= 20 * 1000UL)  // Timeout nach 20 Sekunden
        {
            e_str = "\r\nAPN ERROR: No SIM card detected!\r\n";
            strcpy(err_code, e_str);
            ResetModule();
            return false;
        }
    }
    
     // Schritt 2: APN-Konfiguration setzen
    if (!SetDevAPNParameters(pdp_index, IPV4, apn, usr, pwd, PAP_OR_CHAP))
    {
        e_str = "\r\nAPN ERROR: Failed to set APN parameters!\r\n";
        strcpy(err_code, e_str);
        return false;
    }
    // Schritt 3: Netzregistrierung prüfen
    start_time = millis();
    while (i_status != REGISTERED && i_status != REGISTERED_ROAMING)
    {
        i_status = DevNetRegistrationStatus();
        if (millis() - start_time >= 90 * 1000UL)  // Timeout nach 90 Sekunden
        {
            e_str = "\r\nAPN ERROR: Can't register to the operator network!\r\n";
            strcpy(err_code, e_str);
            ResetModule();
            return false;
        }
        delay(3000);  // Warte 3 Sekunden vor dem nächsten Registrierungsversuch
    }
   
    // Schritt 4: APN aktivieren und IP-Adresse abrufen
    start_time = millis();
    while (millis() - start_time <= 150 * 1000UL)  // Timeout nach 150 Sekunden
    {
        init_status = ActivateDevAPN(pdp_index);
        if (init_status == SUCCESS_RESPONSE)
        {
            if (GetDevAPNIPAddress(pdp_index, i_ip))
            {
                sprintf(err_code, "\r\nAPN OK: The IP address is %s\r\n", i_ip);
                return true;
            }
            else
            {
                e_str = "\r\nAPN ERROR: Failed to retrieve IP address!\r\n";
                strcpy(err_code, e_str);
                return false;
            }
        }
        else if (init_status == TIMEOUT_RESPONSE)
        {
            e_str = "\r\nAPN ERROR: APN activation timeout. Please reset your device!\r\n";
            strcpy(err_code, e_str);
            ResetModule();
            return false;
        }
    }
    // Falls die APN-Aktivierung fehlschlägt
    e_str = "\r\nAPN ERROR: Failed to activate APN!\r\n";
    strcpy(err_code, e_str);
    return false;
}

bool _BG96_TCPIP::TurnOnInternet(unsigned int pdp_index, char* err_code){
    Net_Status_t i_status = NOT_REGISTERED;
    Cmd_Response_t init_status;
    const char *e_str;
    
    unsigned long start_time = millis();
   
    start_time = millis();
    while (i_status != REGISTERED && i_status != REGISTERED_ROAMING)
    {
        i_status = DevNetRegistrationStatus();
        if (millis() - start_time >= 90 * 1000UL)  // Timeout nach 90 Sekunden
        {
            e_str = "\r\nAPN ERROR: Can't register to the operator network!\r\n";
            strcpy(err_code, e_str);
            ResetModule();
            return false;
        }
        delay(3000); 
    }
   
    start_time = millis();
    while (millis() - start_time <= 150 * 1000UL)  // Timeout nach 150 Sekunden
    {
        init_status = ActivateDevAPN(pdp_index);
        if (init_status == SUCCESS_RESPONSE)
        {
           return true;
        }
        else if (init_status == TIMEOUT_RESPONSE)
        {
            e_str = "\r\nAPN ERROR: APN activation timeout. Please reset your device!\r\n";
            strcpy(err_code, e_str);
            ResetModule();
            return false;
        }
    }
    // Falls die APN-Aktivierung fehlschlägt
    e_str = "\r\nAPN ERROR: Failed to activate APN!\r\n";
    strcpy(err_code, e_str);
    return false;
}

bool _BG96_TCPIP::InitAPNWithNetworkScanning(unsigned int pdp_index, const char* apn, const char* usr, const char* pwd, char* err_code, const char* rat, Cell* cells[])
{
    // Statusvariablen
    Net_Status_t i_status = NOT_REGISTERED;
    Cmd_Response_t init_status;
    const char *e_str;
    char i_ip[16];  // Buffer für die IP-Adresse
    // Schritt 1: SIM-Kartenprüfung
    unsigned long start_time = millis();
    while (!DevSimPIN("", READ_MODE))
    {
        if (millis() - start_time >= 20 * 1000UL)  // Timeout nach 20 Sekunden
        {
            e_str = "\r\nAPN ERROR: No SIM card detected!\r\n";
            strcpy(err_code, e_str);
            ResetModule();
            return false;
        }
    }
     // Schritt 2: APN-Konfiguration setzen
    if (!SetDevAPNParameters(pdp_index, IPV4, apn, usr, pwd, PAP_OR_CHAP))
    {
        e_str = "\r\nAPN ERROR: Failed to set APN parameters!\r\n";
        strcpy(err_code, e_str);
        return false;
    }
    ScanCells(rat,cells);
    // Schritt 3: Netzregistrierung prüfen
    start_time = millis();
    while (i_status != REGISTERED && i_status != REGISTERED_ROAMING)
    {
        i_status = DevNetRegistrationStatus();
        if (millis() - start_time >= 90 * 1000UL)  // Timeout nach 90 Sekunden
        {
            e_str = "\r\nAPN ERROR: Can't register to the operator network!\r\n";
            strcpy(err_code, e_str);
            ResetModule();
            return false;
        }
        delay(3000);  // Warte 3 Sekunden vor dem nächsten Registrierungsversuch
    }
   
    // Schritt 4: APN aktivieren und IP-Adresse abrufen
    start_time = millis();
    while (millis() - start_time <= 150 * 1000UL)  // Timeout nach 150 Sekunden
    {
        init_status = ActivateDevAPN(pdp_index);
        if (init_status == SUCCESS_RESPONSE)
        {
            if (GetDevAPNIPAddress(pdp_index, i_ip))
            {
                sprintf(err_code, "\r\nAPN OK: The IP address is %s\r\n", i_ip);
                return true;
            }
            else
            {
                e_str = "\r\nAPN ERROR: Failed to retrieve IP address!\r\n";
                strcpy(err_code, e_str);
                return false;
            }
        }
        else if (init_status == TIMEOUT_RESPONSE)
        {
            e_str = "\r\nAPN ERROR: APN activation timeout. Please reset your device!\r\n";
            strcpy(err_code, e_str);
            ResetModule();
            return false;
        }
    }
    // Falls die APN-Aktivierung fehlschlägt
    e_str = "\r\nAPN ERROR: Failed to activate APN!\r\n";
    strcpy(err_code, e_str);
    return false;
}
/**
 * @brief Öffnet einen Socket-Dienst mit den angegebenen Parametern.
 * 
 * @param pdp_index Index des PDP-Kontexts
 * @param socket_index Index des Sockets
 * @param socket Typ des Sockets (TCP/UDP Client/Server)
 * @param ip Ziel-IP-Adresse
 * @param port Zielport
 * @param local_port Lokaler Port
 * @param mode Zugriffsmodus
 * @return true bei erfolgreichem Öffnen, sonst false
 */
bool _BG96_TCPIP::OpenSocketService(unsigned int pdp_index, unsigned int socket_index, Socket_Type_t socket, char *ip, unsigned int port, unsigned int local_port, Access_Mode_t mode)
{
    char cmd[128], buf[128];
    strcpy(cmd, OPEN_SOCKET);
    switch(socket)
    {
        case TCP_CLIENT:
            sprintf(buf, "=%d,%d,\"TCP\",\"%s\",%d,%d,%d", pdp_index, socket_index, ip, port, local_port, mode);
            break;
        case TCP_SEVER:
            sprintf(buf, "=%d,%d,\"TCP LISTENER\",\"%s\",%d,%d,%d", pdp_index, socket_index, ip, port, local_port, mode);
            break;
        case UDP_CLIENT:
            sprintf(buf, "=%d,%d,\"UDP\",\"%s\",%d,%d,%d", pdp_index, socket_index, ip, port, local_port, mode);
            break;
        case UDP_SEVER:
            sprintf(buf, "=%d,%d,\"UDP SERVICE\",\"%s\",%d,%d,%d", pdp_index, socket_index, ip, port, local_port, mode);
            break;
        default:
            return false;
    }
    strcat(cmd, buf);
    switch(mode)
    {
        case BUFFER_MODE:
        case DIRECT_PUSH_MODE:
            if (sendAndSearch(cmd, OPEN_SOCKET, RESPONSE_ERROR, 150) > 0){
                unsigned long start_time = millis();
                while(millis() - start_time < 200UL){
                    if (serialAvailable()){
                        readResponseByteToBuffer();
                    }
                }
                errorCode = -1;
                char *sta_buf = searchChrBuffer(',');
                if(atoi(sta_buf + 1) == 0){
                    return true;
                }else {
                    errorCode = atoi(sta_buf + 1);
                }
            }
            break;
        case TRANSPARENT_MODE:
            if (sendAndSearch(cmd, RESPONSE_CONNECT, RESPONSE_ERROR, 150) > 0){
                return true;
            }
            break;
        default:
            break;
    }
    return false;
}

/**
 * @brief Schließt den angegebenen Socket-Dienst.
 * 
 * @param socket_index Index des Sockets
 * @return true bei erfolgreichem Schließen, sonst false
 */
bool _BG96_TCPIP::CloseSocketService(unsigned int socket_index)
{
    char cmd[16], buf[8];
    strcpy(cmd, CLOSE_SOCKET);
    sprintf(buf, "=%d", socket_index);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 150)){
        return true;
    }
    return false;
}

/**
 * @brief Sendet Daten über den angegebenen Socket.
 * 
 * @param socket_index Index des Sockets
 * @param socket Typ des Sockets (TCP/UDP)
 * @param data_buf Puffer mit den zu sendenden Daten
 * @param ip Ziel-IP-Adresse (für UDP-Server)
 * @param port Zielport (für UDP-Server)
 * @return true bei erfolgreichem Senden, sonst false
 */
bool _BG96_TCPIP::SocketSendData(unsigned int socket_index, Socket_Type_t socket, const char *data_buf, const char *ip, unsigned int port)
{
    char cmd[64], buf[64];
    strcpy(cmd, SOCKET_SEND_DATA);
    
    switch (socket)
    {
        case TCP_CLIENT:
        case TCP_SEVER:
        case UDP_CLIENT:
            sprintf(buf, "=%d,%d", socket_index, strlen(data_buf));
            break;
        case UDP_SEVER:
            sprintf(buf, "=%d,%d,\"%s\",%d", socket_index, strlen(data_buf), ip, port);
            break;
        default:
            return false;
    }
    
    strcat(cmd, buf);
    
    if (sendAndSearchChr(cmd, '>', 2))
    {
        if (sendDataAndCheck(data_buf, RESPONSE_SEND_OK, RESPONSE_SEND_FAIL, 10))
        {
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Empfängt Daten über den angegebenen Socket.
 * 
 * @param socket_index Index des Sockets
 * @param recv_len Anzahl der zu empfangenden Bytes
 * @param socket Typ des Sockets (TCP/UDP)
 * @param recv_buf Puffer zur Speicherung der empfangenen Daten
 * @return true bei erfolgreichem Empfang, sonst false
 */
bool _BG96_TCPIP::SocketRecvData(unsigned int socket_index, unsigned int recv_len, Socket_Type_t socket, char *recv_buf)
{
    char cmd[16], buf[16];
    strcpy(cmd, SOCKET_READ_DATA);
    switch(socket)
    {
        case TCP_CLIENT:
        case TCP_SEVER:
        case UDP_CLIENT:
            sprintf(buf, "=%d,%d", socket_index, recv_len);
            break;
        case UDP_SEVER:
            sprintf(buf, "=%d", socket_index);
            break;
        default:
            return false;
    }
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_CRLF_OK, RESPONSE_ERROR, 10)){
        char *sta_buf = searchStrBuffer(": "); 
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        strcpy(recv_buf, sta_buf + 2);
        return true;
    }
    return false;
}

/**
 * @brief Sendet HEX-codierte Daten über den angegebenen Socket.
 * 
 * @param socket_index Index des Sockets
 * @param hex_buf Puffer mit den HEX-Daten
 * @return true bei erfolgreichem Senden, sonst false
 */
bool _BG96_TCPIP::SocketSendHEXData(unsigned int socket_index, char *hex_buf)
{
    char cmd[256], buf[256];
    strcpy(cmd, SOCKET_SEND_HEX_DATA);
    sprintf(buf, "=%d,\"%s\"", socket_index, hex_buf);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_SEND_OK, RESPONSE_SEND_FAIL, 2)){
        return true;
    }
    return false;
}

/**
 * @brief Wechselt den Zugriffsmodus des angegebenen Sockets.
 * 
 * @param socket_index Index des Sockets
 * @param mode Neuer Zugriffsmodus
 * @return true bei erfolgreichem Wechsel, sonst false
 */
bool _BG96_TCPIP::SwitchAccessModes(unsigned int socket_index, Access_Mode_t mode)
{
    char cmd[16], buf[16];
    strcpy(cmd, DATA_ACCESS_MODES);
    sprintf(buf, "=%d,%d", socket_index, mode);
    strcat(cmd, buf);
    if (mode == TRANSPARENT_MODE){
        if(sendAndSearch(cmd, RESPONSE_CONNECT, RESPONSE_ERROR, 2)){
            return true;
        }
    } else {
        if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
            return true;
        }
    }
    return false;
}

/**
 * @brief Führt eine NTP-Anfrage über den angegebenen Socket aus.
 * 
 * @param socket_index Index des Sockets
 * @param ntp_ip NTP-Server-IP-Adresse
 * @param port NTP-Server-Port
 * @param time Puffer zur Speicherung der abgerufenen Zeit
 * @return true bei erfolgreicher Anfrage, sonst false
 */
bool _BG96_TCPIP::DevNTPFunction(unsigned int socket_index, char *ntp_ip, unsigned int port, char *time)
{
    char cmd[64], buf[64];
    strcpy(cmd, NTP_FUNCTION);
    sprintf(buf, "=%d,\"%s\",%d", socket_index, ntp_ip, port);
    strcat(cmd, buf);
    if (sendAndSearch(cmd, NTP_FUNCTION, RESPONSE_ERROR, 125)){
        unsigned long start_time = millis();
        while(millis() - start_time < 200UL){
            if (serialAvailable()){
                readResponseByteToBuffer();
            }
        }
        char *sta_buf = searchStrBuffer(": ");
        strcpy(time, sta_buf + 2);
        return true;
    }
    return false;
}

/**
 * @brief Konfiguriert die DNS-Server-Adressen für den angegebenen Socket.
 * 
 * @param socket_index Index des Sockets
 * @param primary_addr Primäre DNS-Server-Adresse
 * @param secondary_addr Sekundäre DNS-Server-Adresse
 * @return true bei erfolgreicher Konfiguration, sonst false
 */
bool _BG96_TCPIP::ConfigDNSServer(unsigned int socket_index, char *primary_addr, char *secondary_addr)
{
    char cmd[128], buf[128];
    strcpy(cmd, CONFIGURE_DNS_SERVER);
    sprintf(buf, "=%d,\"%s\",\"%s\"", socket_index, primary_addr, secondary_addr);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        return true;
    }
    return false;
}

/**
 * @brief Führt eine DNS-Auflösung für den angegebenen Domainnamen durch.
 * 
 * @param socket_index Index des Sockets
 * @param domain_name Zu lösendender Domainname
 * @param ip Puffer zur Speicherung der aufgelösten IP-Adresse
 * @return true bei erfolgreicher Auflösung, sonst false
 */
bool _BG96_TCPIP::DevDNSFunction(unsigned int socket_index, char *domain_name, char *ip)
{
    char cmd[128], buf[128];
    strcpy(cmd, DNS_FUNCTION);
    sprintf(buf, "=%d,\"%s\"", socket_index, domain_name);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, "+QIURC", RESPONSE_ERROR, 60)){
        unsigned long start_time = millis();
        while(millis() - start_time < 200UL){
            if (serialAvailable()){
                readResponseByteToBuffer();
            }
        }
        char *sta_buf = searchStrBuffer(": ");
        strcpy(ip, sta_buf + 2);
        return true;
    }
    return false;
}

/**
 * @brief Fragt den letzten Fehlercode vom Gerät ab.
 * 
 * @param err_code Puffer zur Speicherung des Fehlercodes
 * @return true bei erfolgreichem Abruf, sonst false
 */
bool _BG96_TCPIP::QueryLastErrorCode(char *err_code)
{
    char cmd[16];
    strcpy(cmd, QUERY_ERROR_CODE);
    if(sendAndSearch(cmd, RESPONSE_OK, 2)){
        char *sta_buf = searchStrBuffer(": ");
        strcpy(err_code, sta_buf + 2);
        return true;
    }
    return false;
}

/**
 * @brief Wartet und prüft auf Socket-Ereignisse innerhalb eines angegebenen Zeitlimits.
 * 
 * @param event Puffer zur Speicherung des Ereignistyps
 * @param timeout Zeitlimit in Millisekunden
 * @return Socket_Event_t Typ des empfangenen Ereignisses
 */
Socket_Event_t _BG96_TCPIP::WaitCheckSocketEvent(char *event, unsigned int timeout)
{
    if(readResponseAndSearch(RECV_SOCKET_EVENT, timeout)){
        unsigned long start_time = millis();
        while(millis() - start_time < 200UL){
            if (serialAvailable()){
                readResponseByteToBuffer();
            }
        }
        char *sta_buf = searchChrBuffer(',');
        strcpy(event, sta_buf + 1);
        if(searchStrBuffer("closed")){
            return SOCKET_CLOSE_EVENT;
        } else if (searchStrBuffer("recv")){
            return SOCKET_RECV_DATA_EVENT;
        } else if (searchStrBuffer("incoming full")){
            return SOCKET_CONNECTION_FULL_EVENT;
        } else if (searchStrBuffer("incoming")){
            return SOCKET_INCOMING_CONNECTION_EVENT;
        } else if (searchStrBuffer("pdpdeact")){
            return SOCKET_PDP_DEACTIVATION_EVENT;
        }
    }
    return (Socket_Event_t)0;
}
