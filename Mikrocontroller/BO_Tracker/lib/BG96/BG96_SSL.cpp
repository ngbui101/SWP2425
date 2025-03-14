#include "BG96_SSL.h"

/**
 * @brief Konstruktor der Klasse _BG96_SSL.
 *
 * Initialisiert die Klasse _BG96_SSL ohne zusätzliche Konfiguration.
 */
_BG96_SSL::_BG96_SSL()
{
}

/**
 * @brief Destruktor der Klasse _BG96_SSL.
 *
 * Bereinigt den Puffer beim Zerstören des Objekts.
 */
_BG96_SSL::~_BG96_SSL()
{
    cleanBuffer();
}

/**
 * @brief Konstruktor mit seriellen Schnittstellen.
 *
 * @param atserial Stream für AT-Befehle
 * @param dserial Stream für Debug-Ausgaben
 */
_BG96_SSL::_BG96_SSL(Stream &atserial, Stream &dserial) : _BG96_FILE(atserial, dserial)
{
}

/**
 * @brief Setzt die SSL-Parameter für die angegebene SSL-Instanz.
 *
 * Diese Methode basiert auf dem AT-Befehl AT+QSSLCFG, der zur Konfiguration der SSL-Parameter, wie SSL-Version, Cipher-Suites und Verhandlungstimeout verwendet wird.
 *
 * @param ssl_index Index des SSL-Kontexts
 * @param s_version SSL-Version (z.B. TLS 1.2)
 * @param s_cipher Verschlüsselungssuite (z.B. RSA mit AES)
 * @param negotiation_time Verhandlungstimeout in Sekunden
 * @return true bei Erfolg, sonst false
 *
 * @note Siehe "BG96 SSL Application Note", Abschnitt 2.1.1 AT+QSSLCFG, Seite 10.
 */

bool _BG96_SSL::SetSSLParameters(unsigned int ssl_index, SSL_Version_t s_version, SSL_Cipher_Suites_t s_ciper, unsigned int negotiation_time)
{
    char cmd[64], buf[32];
    strcpy(cmd, SSL_CONFIG_PARAMETER);
    sprintf(buf, "=\"sslversion\",%d,%d", ssl_index, s_version);
    strcat(cmd, buf);
    if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5))
    {
        memset(cmd, '\0', 64);
        memset(buf, '\0', 32);
        strcpy(cmd, SSL_CONFIG_PARAMETER);
        switch (s_ciper)
        {
        case TLS_RSA_WITH_AES_256_CBC_SHA:
            sprintf(buf, "=\"ciphersuite\",%d,0X0035", ssl_index);
            break;
        case TLS_RSA_WITH_AES_128_CBC_SHA:
            sprintf(buf, "=\"ciphersuite\",%d,0X002F", ssl_index);
            break;
        case TLS_RSA_WITH_RC4_128_SHA:
            sprintf(buf, "=\"ciphersuite\",%d,0X0005", ssl_index);
            break;
        case TLS_RSA_WITH_RC4_128_MD5:
            sprintf(buf, "=\"ciphersuite\",%d,0X0004", ssl_index);
            break;
        case TLS_RSA_WITH_3DES_EDE_CBC_SHA:
            sprintf(buf, "=\"ciphersuite\",%d,0X000A", ssl_index);
            break;
        case TLS_RSA_WITH_AES_256_CBC_SHA256:
            sprintf(buf, "=\"ciphersuite\",%d,0X003D", ssl_index);
            break;
        case TLS_ECDHE_RSA_WITH_RC4_128_SHA:
            sprintf(buf, "=\"ciphersuite\",%d,0XC011", ssl_index);
            break;
        case TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA:
            sprintf(buf, "=\"ciphersuite\",%d,0XC012", ssl_index);
            break;
        case TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA:
            sprintf(buf, "=\"ciphersuite\",%d,0XC013", ssl_index);
            break;
        case TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA:
            sprintf(buf, "=\"ciphersuite\",%d,0XC014", ssl_index);
            break;
        case TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256:
            sprintf(buf, "=\"ciphersuite\",%d,0C027", ssl_index);
            break;
        case TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384:
            sprintf(buf, "=\"ciphersuite\",%d,0XC028", ssl_index);
            break;
        case TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256:
            sprintf(buf, "=\"ciphersuite\",%d,0XC02F", ssl_index);
            break;
        default:
            sprintf(buf, "=\"ciphersuite\",%d,0xFFFF", ssl_index);
            break;
        }
        strcat(cmd, buf);
        if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5))
        {
            memset(cmd, '\0', 64);
            memset(buf, '\0', 32);
            strcpy(cmd, SSL_CONFIG_PARAMETER);
            sprintf(buf, "=\"negotiatetime\",%d,%d", ssl_index, negotiation_time);
            strcat(cmd, buf);
            if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5))
            {
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief Konfiguriert die SSL-Zertifikate.
 *
 * @param ssl_index Index des SSL-Kontexts
 * @param ca_cert_path Pfad zur CA-Zertifikatdatei
 * @param client_cert_path Pfad zum Client-Zertifikat
 * @param client_key_path Pfad zum Client-Schlüssel
 * @param validity_check Überprüfung der Gültigkeit aktivieren
 * @return true bei Erfolg, sonst false
 *
 * @note Siehe "BG96 SSL Application Note", Abschnitt 2.1.1 AT+QSSLCFG, Seite 10.
 */

bool _BG96_SSL::SetSSLCertificate(unsigned int ssl_index, const char *ca_cert_path, const char *client_cert_path, const char *client_key_path, bool validity_check)
{
    char cmd[64], buf[64];
    strcpy(cmd, SSL_CONFIG_PARAMETER);
    if (strcmp(ca_cert_path, "") == 0 && strcmp(client_cert_path, "") == 0 && strcmp(client_key_path, "") == 0)
    {
        sprintf(buf, "=\"seclevel\",%d,0", ssl_index);
        strcat(cmd, buf);
        if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5))
        {
            return true;
        }
    }
    else if (strcmp(ca_cert_path, "") != 0 && strcmp(client_cert_path, "") == 0 && strcmp(client_key_path, "") == 0)
    {
        sprintf(buf, "=\"seclevel\",%d,1", ssl_index);
        strcat(cmd, buf);
        if (!sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5))
        {
            return false;
        }
        memset(cmd, '\0', 64);
        memset(buf, '\0', 32);
        strcpy(cmd, SSL_CONFIG_PARAMETER);
        sprintf(buf, "=\"cacert\",%d,\"%s\"", ssl_index, ca_cert_path);
        strcat(cmd, buf);
        if (!sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5))
        {
            return false;
        }
        memset(cmd, '\0', 64);
        memset(buf, '\0', 32);
        strcpy(cmd, SSL_CONFIG_PARAMETER);
        if (validity_check)
        {
            sprintf(buf, "=\"ignorelocaltime\",2,1");
            strcat(cmd, buf);
            if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5))
            {
                return true;
            }
        }
        else
        {
            sprintf(buf, "=\"ignorelocaltime\",%d,0", ssl_index);
            strcat(cmd, buf);
            if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5))
            {
                return true;
            }
        }
    }
    else if (strcmp(ca_cert_path, "") != 0 && strcmp(client_cert_path, "") != 0 && strcmp(client_key_path, "") != 0)
    {
        sprintf(buf, "=\"seclevel\",%d,2", ssl_index);
        strcat(cmd, buf);
        if (!sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 10))
        {
            return false;
        }
        memset(cmd, '\0', 64);
        memset(buf, '\0', 64);
        strcpy(cmd, SSL_CONFIG_PARAMETER);
        sprintf(buf, "=\"cacert\",%d,\"%s\"", ssl_index, ca_cert_path);
        strcat(cmd, buf);
        if (!sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5))
        {
            return false;
        }
        memset(cmd, '\0', 64);
        memset(buf, '\0', 64);
        strcpy(cmd, SSL_CONFIG_PARAMETER);
        sprintf(buf, "=\"clientcert\",%d,\"%s\"", ssl_index, client_cert_path);
        strcat(cmd, buf);
        if (!sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5))
        {
            return false;
        }
        memset(cmd, '\0', 64);
        memset(buf, '\0', 64);
        strcpy(cmd, SSL_CONFIG_PARAMETER);
        sprintf(buf, "=\"clientkey\",%d,\"%s\"", ssl_index, client_key_path);
        strcat(cmd, buf);
        if (!sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5))
        {
            return false;
        }
        memset(cmd, '\0', 64);
        memset(buf, '\0', 64);
        strcpy(cmd, SSL_CONFIG_PARAMETER);
        if (validity_check)
        {
            sprintf(buf, "=\"ignorelocaltime\",%d,1", ssl_index);
            strcat(cmd, buf);
            if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5))
            {
                return true;
            }
        }
        else
        {
            sprintf(buf, "=\"ignorelocaltime\",1");
            strcat(cmd, buf);
            if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5))
            {
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief Initialisiert SSL, lädt Zertifikate und setzt Parameter.
 *
 * AT-Befehl: AT+QSSLCFG (siehe Seite 12–13 in der Dokumentation)
 *
 * @param ssl_index Index des SSL-Kontexts
 * @param ca_cert Pfad zum CA-Zertifikat
 * @param client_cert Pfad zum Client-Zertifikat
 * @param client_key Pfad zum Client-Schlüssel
 * @param err_code Puffer für eventuelle Fehlermeldungen
 * @return true bei Erfolg, sonst false
 */

bool _BG96_SSL::InitSSL(unsigned int ssl_index, const char *ca_cert,const char *client_cert,const char *client_key, char *err_code)
{
    unsigned long start_time;
    int f_err_code;

    // Case 1: No certificates provided (non-secure)
    if (strcmp(ca_cert, "") == 0 && strcmp(client_cert, "") == 0 && strcmp(client_key, "") == 0)
    {
        if (SetSSLCertificate(ssl_index, "", "", "", false))
        {
            strcpy(err_code, "\r\nSSL OK: The SSL was successfully initialized without certificates.\r\n");
            return true;
        }
        strcpy(err_code, "\r\nSSL ERROR: An error occurred while setting the SSL certificates.\r\n");
        return false;
    }
    // Case 2: Only CA certificate provided (one-way SSL)
    else if (strcmp(ca_cert, "") != 0 && strcmp(client_cert, "") == 0 && strcmp(client_key, "") == 0)
    {
        // Upload CA certificate
        start_time = millis();
        while (!UploadFiles((char *)ssl_ca_cert_name, ca_cert))
        {
            if (returnErrorCode(f_err_code))
            {
                if (f_err_code == 407) // File already exists
                {
                    unsigned long delete_start_time = millis();
                    while (!DeleteFiles((char *)ssl_ca_cert_name))
                    {
                        if (millis() - delete_start_time >= 10 * 1000UL)
                        {
                            strcpy(err_code, "\r\nSSL ERROR: Unable to delete existing CA certificate file during re-upload.\r\n");
                            return false;
                        }
                    }
                }
                else
                {
                    sprintf(err_code, "\r\nSSL ERROR: Error uploading CA certificate file, error code: %d.\r\n", f_err_code);
                    return false;
                }
            }
            if (millis() - start_time >= 30 * 1000UL)
            {
                strcpy(err_code, "\r\nSSL ERROR: Timeout occurred while uploading CA certificate file.\r\n");
                return false;
            }
        }

        // Set SSL certificate with only CA certificate
        start_time = millis();
        while (!SetSSLCertificate(ssl_index, (char *)ssl_ca_cert_name, "", "", false))
        {
            if (millis() - start_time >= 30 * 1000UL)
            {
                strcpy(err_code, "\r\nSSL ERROR: An error occurred while setting the SSL certificates.\r\n");
                return false;
            }
        }
        strcpy(err_code, "\r\nSSL OK: The SSL was successfully initialized with CA certificate.\r\n");
        return true;
    }
    // Case 3: CA certificate, client certificate, and client key provided (two-way SSL)
    else if (strcmp(ca_cert, "") != 0 && strcmp(client_cert, "") != 0 && strcmp(client_key, "") != 0)
    {
        // Upload CA certificate
        start_time = millis();
        while (!UploadFiles((char *)ssl_ca_cert_name, ca_cert))
        {
            if (returnErrorCode(f_err_code))
            {
                if (f_err_code == 407) // File already exists
                {
                    unsigned long delete_start_time = millis();
                    while (!DeleteFiles((char *)ssl_ca_cert_name))
                    {
                        if (millis() - delete_start_time >= 10 * 1000UL)
                        {
                            strcpy(err_code, "\r\nSSL ERROR: Unable to delete existing CA certificate file during re-upload.\r\n");
                            return false;
                        }
                    }
                }
                else
                {
                    sprintf(err_code, "\r\nSSL ERROR: Error uploading CA certificate file, error code: %d.\r\n", f_err_code);
                    return false;
                }
            }
            if (millis() - start_time >= 30 * 1000UL)
            {
                strcpy(err_code, "\r\nSSL ERROR: Timeout occurred while uploading CA certificate file.\r\n");
                return false;
            }
        }

        // Upload Client Certificate
        start_time = millis();
        while (!UploadFiles((char *)ssl_client_cert_name, client_cert))
        {
            if (returnErrorCode(f_err_code))
            {
                if (f_err_code == 407) // File already exists
                {
                    unsigned long delete_start_time = millis();
                    while (!DeleteFiles((char *)ssl_client_cert_name))
                    {
                        if (millis() - delete_start_time >= 10 * 1000UL)
                        {
                            strcpy(err_code, "\r\nSSL ERROR: Unable to delete existing client certificate file during re-upload.\r\n");
                            return false;
                        }
                    }
                }
                else
                {
                    sprintf(err_code, "\r\nSSL ERROR: Error uploading client certificate file, error code: %d.\r\n", f_err_code);
                    return false;
                }
            }
            if (millis() - start_time >= 30 * 1000UL)
            {
                strcpy(err_code, "\r\nSSL ERROR: Timeout occurred while uploading client certificate file.\r\n");
                return false;
            }
        }

        // Upload Client Key
        start_time = millis();
        while (!UploadFiles((char *)ssl_client_key_name, client_key))
        {
            if (returnErrorCode(f_err_code))
            {
                if (f_err_code == 407) // File already exists
                {
                    unsigned long delete_start_time = millis();
                    while (!DeleteFiles((char *)ssl_client_key_name))
                    {
                        if (millis() - delete_start_time >= 10 * 1000UL)
                        {
                            strcpy(err_code, "\r\nSSL ERROR: Unable to delete existing client key file during re-upload.\r\n");
                            return false;
                        }
                    }
                }
                else
                {
                    sprintf(err_code, "\r\nSSL ERROR: Error uploading client key file, error code: %d.\r\n", f_err_code);
                    return false;
                }
            }
            if (millis() - start_time >= 30 * 1000UL)
            {
                strcpy(err_code, "\r\nSSL ERROR: Timeout occurred while uploading client key file.\r\n");
                return false;
            }
        }

        // Set SSL parameters
        // AWS IoT Cores Security policy TLS13_1_2_2022_10 only supports ECDHE-RSA-AES256-SHA384 for BG96
        if (!SetSSLParameters(ssl_index, TLS_1_2, SUPPORT_ALL_ABOVE, 300))
        {
            strcpy(err_code, "\r\nSSL ERROR: An error occurred while setting the SSL parameters.\r\n");
            return false;
        }
        
        // Set SSL Certificates
        start_time = millis();
        while (!SetSSLCertificate(ssl_index, (char *)ssl_ca_cert_name, (char *)ssl_client_cert_name, (char *)ssl_client_key_name, false))
        {
            if (millis() - start_time >= 30 * 1000UL)
            {
                strcpy(err_code, "\r\nSSL ERROR: An error occurred while setting the SSL certificates.\r\n");
                return false;
            }
        }

        strcpy(err_code, "\r\nSSL OK: The SSL was successfully initialized with CA certificate, client certificate, and client key.\r\n");
        return true;
    }
    else
    {
        strcpy(err_code, "\r\nSSL ERROR: Invalid certificate parameters provided.\r\n");
        return false;
    }
}

/**
 * @brief Öffnet einen SSL-Socket.
 *
 * AT-Befehl: AT+QSSLOPEN (siehe Seite 15–16 in der Dokumentation)
 *
 * @param pdp_index Index des PDP-Kontexts
 * @param ssl_index Index des SSL-Kontexts
 * @param socket_index Index des Sockets
 * @param ip Ziel-IP-Adresse
 * @param port Zielport
 * @param mode Zugriffsmodus (z.B. BUFFER_MODE)
 * @return true bei Erfolg, sonst false
 */

bool _BG96_SSL::OpenSSLSocket(unsigned int pdp_index, unsigned int ssl_index, unsigned int socket_index, char *ip, unsigned int port, Access_Mode_t mode)
{
    char cmd[64], buf[64];
    strcpy(cmd, SSL_OPEN_SOCKET);
    sprintf(buf, "=%d,%d,%d,\"%s\",%d,%d", pdp_index, ssl_index, socket_index, ip, port, mode);
    strcat(cmd, buf);
    switch (mode)
    {
    case BUFFER_MODE:
    case DIRECT_PUSH_MODE:
        if (sendAndSearch(cmd, SSL_OPEN_SOCKET, RESPONSE_ERROR, 150) > 0)
        {
            unsigned long start_time = millis();
            while (millis() - start_time < 200UL)
            {
                if (serialAvailable())
                {
                    readResponseByteToBuffer();
                }
            }
            char *sta_buf = searchChrBuffer(',');
            errorCode = -1;
            if (atoi(sta_buf + 1) == 0)
            {
                return true;
            }
            else
            {
                errorCode = atoi(sta_buf + 1);
            }
        }
        break;
    case TRANSPARENT_MODE:
        if (sendAndSearch(cmd, RESPONSE_CONNECT, RESPONSE_ERROR, 150) > 0)
        {
            return true;
        }
        break;
    default:
        break;
    }
    return false;
}

/**
 * @brief Sendet Daten über den SSL-Socket.
 *
 * AT-Befehl: AT+QSSLSEND (siehe Seite 17 in der Dokumentation)
 *
 * @param socket_index Index des Sockets
 * @param ssl_send_data Zu sendende Daten
 * @return true bei Erfolg, sonst false
 */

bool _BG96_SSL::SSLSocketSendData(unsigned int socket_index, char *ssl_send_data)
{
    char cmd[16], buf[32]; // Increased buffer size
    strcpy(cmd, SSL_SEND_DATA);
    sprintf(buf, "=%d,%d", socket_index, strlen(ssl_send_data));
    strcat(cmd, buf);
    if (sendAndSearchChr(cmd, '>', 2))
    {
        if (sendDataAndCheck(ssl_send_data, RESPONSE_SEND_OK, RESPONSE_SEND_FAIL, 30))
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Empfängt Daten über den SSL-Socket.
 *
 * AT-Befehl: AT+QSSLRECV (siehe Seite 18 in der Dokumentation)
 *
 * @param socket_index Index des Sockets
 * @param ssl_recv_len Anzahl der zu empfangenden Bytes
 * @param ssl_recv_data Puffer für empfangene Daten
 * @return true bei Erfolg, sonst false
 */

bool _BG96_SSL::SSLSocketRecvData(unsigned int socket_index, unsigned int ssl_recv_len, char *ssl_recv_data)
{
    char cmd[16], buf[16];
    strcpy(cmd, SSL_READ_DATA);
    sprintf(buf, "=%d,%d", socket_index, ssl_recv_len);
    strcat(cmd, buf);
    if (sendAndSearch(cmd, RESPONSE_CRLF_OK, RESPONSE_ERROR, 30))
    {
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        char *sta_buf = searchStrBuffer(": ");
        strcpy(ssl_recv_data, sta_buf + 2);
        return true;
    }
    return false;
}

/**
 * @brief Schließt den SSL-Socket.
 *
 * AT-Befehl: AT+QSSLCLOSE (siehe Seite 19 in der Dokumentation)
 *
 * @param socket_index Index des Sockets
 * @return true bei Erfolg, sonst false
 */

bool _BG96_SSL::CloseSSLSocket(unsigned int socket_index)
{
    char cmd[16], buf[16];
    strcpy(cmd, SSL_CLOSE_SOCKET);
    sprintf(buf, "=%d", socket_index);
    strcat(cmd, buf);
    if (sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 150))
    {
        return true;
    }
    return false;
}

/**
 * @brief Fragt den Status des SSL-Sockets ab.
 *
 * AT-Befehl: AT+QSSLSTATE (siehe Seite 19–20 in der Dokumentation)
 *
 * @param socket_index Index des Sockets
 * @param ssl_status Puffer für den Status
 * @return true bei Erfolg, sonst false
 */

bool _BG96_SSL::QuerySSLSocketStatus(unsigned int socket_index, char *ssl_status)
{
    char cmd[16], buf[16];
    strcpy(cmd, SSL_QUERY_STATUS);
    sprintf(buf, "=%d", socket_index);
    strcat(cmd, buf);
    if (sendAndSearch(cmd, RESPONSE_OK, 10))
    {
        char *sta_buf = searchStrBuffer(": ");
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        strcpy(ssl_status, sta_buf + 2);
        return true;
    }
    return false;
}

/**
 * @brief Überwacht SSL-Socket-Ereignisse.
 *
 * AT-Befehl: +QSSLURC (siehe Seite 21–22 in der Dokumentation)
 *
 * @param event Puffer für das Ereignis
 * @param timeout Timeout in Sekunden
 * @return SSL_Socket_Event_t Typ des Ereignisses
 */

SSL_Socket_Event_t _BG96_SSL::WaitCheckSSLSocketEvent(char *event, unsigned int timeout)
{
    if (readResponseAndSearch(SSL_SOCKET_EVENT, timeout))
    {
        unsigned long start_time = millis();
        while (millis() - start_time < 200UL)
        {
            if (serialAvailable())
            {
                readResponseByteToBuffer();
            }
        }
        char *sta_buf = searchChrBuffer(',');
        strcpy(event, sta_buf + 1);
        if (searchStrBuffer("closed"))
        {
            return SSL_SOCKET_CLOSE_EVENT;
        }
        else if (searchStrBuffer("recv"))
        {
            return SSL_SOCKET_RECV_EVENT;
        }
    }
    return (SSL_Socket_Event_t)0;
}

/**
 * @brief  SSL Zertifikat löschen
 * @param filename Name des Zertifikats, "all" falls alle Zertifikate gelöscht werden sollten  
 * @return true bei Erfolg, sonst false
 */

bool _BG96_SSL::DeleteCertificate(const char *filename)
{
    if (strcmp(filename, "ca_cert.pem") == 0)
    {
        DeleteFiles("ca_cert.pem");
        return true;
    }
    else if (strcmp(filename, "client_cert.pem") == 0)
    {
        DeleteFiles("client_cert.pem");
        return true;
    }
    else if (strcmp(filename, "client_key.pem") == 0)  // Korrigierter Dateiname
    {
        DeleteFiles("client_key.pem");
        return true;
    }
    else if (strcmp(filename, "all") == 0)
    {
        DeleteFiles("ca_cert.pem");
        DeleteFiles("client_cert.pem");
        DeleteFiles("client_key.pem");
        return true;
    }
    return false;
}
