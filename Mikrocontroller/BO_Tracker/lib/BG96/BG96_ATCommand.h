/*
*AT Command List für Quectel BG96
*Mehr: 
*quectel_bg96_tcpip_at_commands_manual_v1-1
*quectel_bg96_ssl_application_note_v1-1
*quectel_bg96_queccell_at_commands_manual_v1-0
*quectel_bg96_mqtt_application_note_v1-2
*quectel_bg96_https_at_commands_manual_v1-0
*Quectel_BG96_GNSS_AT_Commands_Manual_V1.1
*quectel_bg96_file_at_commands_manual_v1-1
*Quectel_BG96_AT_Commands_Manual_V2.3
 */

#ifndef __BG96_ATCOMMAND_H_
#define __BG96_ATCOMMAND_H_

// AT commands response
const char RESPONSE_READY[] = "APP RDY";
const char RESPONSE_OK[] = "OK";
const char RESPONSE_CRLF_OK[] = "\r\n\r\nOK";
const char RESPONSE_ERROR[] = "ERROR";
const char RESPONSE_POWER_DOWN[] = "POWERED DOWN";
const char RESPONSE_CONNECT[] = "CONNECT";
const char RESPONSE_SEND_OK[] = "SEND OK";
const char RESPONSE_SEND_FAIL[] = "SEND FAIL";

// common AT commands
const char DEV_AT[] = "";
const char DEV_INFORMATION[] = "I";
const char DEV_OUTPUTFORMAT[] = "V1";
const char DEV_GMTTIME[] = "+QLTS";
const char DEV_VERSION[] = "+CGMR";
const char DEV_IMEI[] = "+CGSN";
const char DEV_FUN_LEVEL[] = "+CFUN";
const char DEV_LOCAL_RATE[] = "+IPR";
const char DEV_SIM_IMSI[] = "+CIMI";
const char DEV_SIM_PIN[] = "+CPIN";
const char DEV_SIM_ICCID[] = "+QCCID";
const char DEV_NET_STATUS[] = "+CREG";
const char DEV_NET_STATUS_G[] = "+CGREG";
const char DEV_EPS_NET_STATUS[] = "+CEREG";
const char DEV_NET_RSSI[] = "+CSQ";
const char DEV_NET_OPERATOR[] = "+COPS";
const char DEV_NET_INFORMATION[] = "+QNWINFO";
const char DEV_NET_PACKET_COUNTER[] = "+QGDCNT";
const char DEV_POWER_DOWN[] = "+QPOWD";
const char DEV_CLOCK[] = "+CCLK";

// TCPIP AT Commands
const char APN_PARAMETERS[] = "+QICSGP";
const char ACTIVATE_APN[] = "+QIACT";
const char DEACTIVATE_APN[] = "+QIDEACT";
const char GET_APN_IP_ADDRESS[] = "+CGPADDR";
const char OPEN_SOCKET[] = "+QIOPEN";
const char CLOSE_SOCKET[] = "+QICLOSE";
const char SOCKET_STATUS[] = "+QISTATE";
const char SOCKET_SEND_DATA[] = "+QISEND";
const char SOCKET_READ_DATA[] = "+QIRD";
const char SOCKET_SEND_HEX_DATA[] = "+QISENDEX";
const char DATA_ACCESS_MODES[] = "+QISWTMD";
const char PING_FUNCTION[] = "+QPING";
const char NTP_FUNCTION[] = "+QNTP";
const char CONFIGURE_DNS_SERVER[] = "+QIDNSCFG";
const char DNS_FUNCTION[] = "+QIDNSGIP";
const char QUERY_ERROR_CODE[] = "+QIGETERROR";
const char RECV_SOCKET_EVENT[] = "+QIURC";
const char ATTACH_PS[] = "+CGATT";
// FILE AT Commands
const char FILE_SPACE_INFORMATION[] = "+QFLDS";
const char FILE_LIST_FILES[] = "+QFLST";
const char FILE_DELETE_FILES[] = "+QFDEL";
const char FILE_UPLOAD_FILES[] = "+QFUPL";
const char FILE_DOWNLOAD_FILE[] = "+QFDWL";
const char FILE_OPEN_FILE[] = "+QFOPEN";
const char FILE_READ_FILE[] = "+QFREAD";
const char FILE_WRITE_FILE[] = "+QFWRITE";
const char FILE_SET_POSITION_FILE[] = "+QFSEEK";
const char FILE_GET_POSITION_FILE[] = "+QFPOSITION";
const char FILE_TRUNCATE_FILE[] = "+QFTUCAT";
const char FILE_CLOSE_FILE[] = "+QFCLOSE";

// SSL AT Commands
const char SSL_CONFIG_PARAMETER[] = "+QSSLCFG";
const char SSL_OPEN_SOCKET[] = "+QSSLOPEN";
const char SSL_SEND_DATA[] = "+QSSLSEND";
const char SSL_READ_DATA[] = "+QSSLRECV";
const char SSL_CLOSE_SOCKET[] = "+QSSLCLOSE";
const char SSL_QUERY_STATUS[] = "+QSSLSTATE";
const char SSL_SOCKET_EVENT[] = "+QSSLURC";

// HTTP AT Commands
const char HTTP_CONFIG_PARAMETER[] = "+QHTTPCFG";
const char HTTP_SET_URL[] = "+QHTTPURL";
const char HTTP_GET_REQUEST[] = "+QHTTPGET";
const char HTTP_POST_REQUEST[] = "+QHTTPPOST";
const char HTTP_FILE_POST_REQUEST[] = "+QHTTPPOSTFILE";
const char HTTP_READ_RESPONSE[] = "+QHTTPREAD";
const char HTTP_FILE_READ_RESPONSE[] = "+QHTTPREADFILE";

// MQTT AT Commands
const char MQTT_CONFIG_PARAMETER[] = "+QMTCFG";
const char MQTT_OPEN_NETWORK[] = "+QMTOPEN";
const char MQTT_CLOSE_NETWORK[] = "+QMTCLOSE";
const char MQTT_CREATE_CLIENT[] = "+QMTCONN";
const char MQTT_CLOSE_CLIENT[] = "+QMTDISC";
const char MQTT_SUBSCRIBE_TOPICS[] = "+QMTSUB";
const char MQTT_UNSUBSCRIBE_TOPICS[] = "+QMTUNS";
const char MQTT_PUBLISH_MESSAGES[] = "+QMTPUB";
const char MQTT_STATUS[] = "+QMTSTAT";
const char MQTT_RECV_DATA[] = "+QMTRECV";

// GNSS AT Commands
const char GNSS_CONFIGURATION[] = "+QGPSCFG";
const char GNSS_TURN_ON[] = "+QGPS";
const char GNSS_TURN_OFF[] = "+QGPSEND";
const char GNSS_GET_POSITION[] = "+QGPSLOC";
const char GNSS_ACQUIRE_NMEA[] = "+QGPSGNMEA";
const char GNSS_ENABLE_GPSONEXTRA[] = "+QGPSXTRA";
const char GNSS_INJECT_GPSONEXTRA_TIME[] = "+QGPSXTRATIME";
const char GNSS_INJECT_GPSONEXTRA_DATA[] = "+QGPSXTRADATA";
const char GNSS_DELETE_ASSISTANCE_DATA[] = "+QGPSDEL";
const char GNSS_GEO_FENCE[] = "+QCFGEXT";
const char GNSS_AGPS_SUPURL[] = "+QGPSSUPLURL";
// Queccell AT Commands
const char QUECCELL_ENGINEERING_MODE[] = "+QENG";
const char QUECCELL_AVAILABLE_NETWORK[] = "+QCOPS";
const char PING[] = "{\"Ping\":\"\"}";
#endif