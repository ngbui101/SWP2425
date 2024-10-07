/*
*Die Klasse _BG96_TCPIP erweitert die Klasse _BG96_Common und bietet Funktionen 
*zur Verwaltung von TCP/IP-Verbindungen über das BG96-Modul. 
*Sie ermöglicht die Konfiguration von APN-Parametern, die Aktivierung von APN-Verbindungen, 
*das Öffnen und Schließen von Socket-Diensten, das Senden und Empfangen 
*von Daten über Sockets sowie zusätzliche Netzwerkfunktionen wie NTP und DNS
 */

#ifndef __BG96_TCPIP_H_
#define __BG96_TCPIP_H_

#include "BG96_Common.h"

typedef enum protocol_type{
    IPV4 = 1,
    IPV4V6 = 2,
}Protocol_Type_t;

typedef enum authentication_methods{
    TCPIP_NONE = 0,
    PAP = 1,
    CHAP = 2,
    PAP_OR_CHAP = 3,
}Authentication_Methods_t;

typedef enum socket_type{
    TCP_CLIENT = 0,
    TCP_SEVER = 1,
    UDP_CLIENT = 2,
    UDP_SEVER = 3,
}Socket_Type_t;

typedef enum access_mode{
    BUFFER_MODE = 0,
    DIRECT_PUSH_MODE = 1,
    TRANSPARENT_MODE = 2,
}Access_Mode_t;

typedef enum socket_event{
    SOCKET_RECV_DATA_EVENT = 1,
    SOCKET_CLOSE_EVENT = 2,
    SOCKET_PDP_DEACTIVATION_EVENT = 3,
    SOCKET_INCOMING_CONNECTION_EVENT = 4,
    SOCKET_CONNECTION_FULL_EVENT = 5,
}Socket_Event_t;

class _BG96_TCPIP : public _BG96_Common
{
  public:
    _BG96_TCPIP();

    ~_BG96_TCPIP();

    _BG96_TCPIP(Stream &atserial, Stream &dserial); 

    bool SetDevAPNParameters(unsigned int pdp_index, Protocol_Type_t type, const char *apn, const char *usr, const char *pwd, Authentication_Methods_t met);

    Cmd_Response_t ActivateDevAPN(unsigned int pdp_index);

    bool DeactivateDevAPN(unsigned int pdp_index);

    bool GetDevAPNIPAddress(unsigned int pdp_index, char *ip);

    bool InitAPN(unsigned int pdp_index, const char* apn, const char* usr, const char* pwd, char* err_code);

    bool OpenSocketService(unsigned int pdp_index, unsigned int socket_index, Socket_Type_t socket, char *ip, unsigned int port, unsigned int local_port, Access_Mode_t mode);

    bool CloseSocketService(unsigned int socket_index);

    bool SocketSendData(unsigned int socket_index, Socket_Type_t socket, const char *data_buf, const char *ip, unsigned int port);

    bool SocketRecvData(unsigned int socket_index, unsigned int recv_len, Socket_Type_t socket, char *recv_buf);

    bool SocketSendHEXData(unsigned int socket_index, char *hex_buf);

    bool SwitchAccessModes(unsigned int socket_index, Access_Mode_t mode);

    bool DevPingFunction(unsigned int socket_index, char *host);

    bool DevNTPFunction(unsigned int socket_index, char *ntp_ip, unsigned int port, char *time);

    bool ConfigDNSServer(unsigned int socket_index, char *primary_addr, char *secondary_addr);

    bool DevDNSFunction(unsigned int socket_index, char *domain_name, char *ip);

    bool QueryLastErrorCode(char *err_code);

    Socket_Event_t WaitCheckSocketEvent(char *event, unsigned int timeout);

  private:
  
};

#endif