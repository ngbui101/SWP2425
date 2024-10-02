
#ifndef __BG96_AWS_H
#define __BG96_AWS_H

#include "BG96_MQTT.h"

// This is StarfieldClass2.crt
// Qualcomm's logic is the root CA must be the rootest one.
// But he ROOT CA provide by Amazon by default is the intermediate one.

char aws_root_ca_pem[] = "-----BEGIN CERTIFICATE-----\n\
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n\
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n\
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n\
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n\
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n\
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n\
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n\
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n\
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n\
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n\
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n\
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n\
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n\
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n\
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n\
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n\
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n\
rqXRfboQnoZsG4q5WTP468SQvvG5\n\
-----END CERTIFICATE-----\n";

// "The certificate for this thing"
char certificate_pem_crt[] = "-----BEGIN CERTIFICATE-----\n\
MIIDWjCCAkKgAwIBAgIVAJnT/G/MFJwqB43UUWCxV9wnStIUMA0GCSqGSIb3DQEB\n\
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n\
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yNDA5MjQxNzQ1\n\
MTNaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n\
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDHHElRqGYSAvviI23f\n\
Qg1wNuf/SwWXs3EKDZLV5JhrjhWrxgrGOThAVKlmzrNxRqx5Lcy1itoqHoEskMY4\n\
tCGGUj0dP6gTAn6MBDCOudFHlQ6NpYZNkYMKIHfBUPK6glxs8EYEGn3tVo9vClye\n\
ORIh68z1EOH3cLqC99TC+XOSjBS+TmxbRNnoL/e2QU9hDwct/gb7pcIrMLB9I+Ca\n\
7o/xyEjGs6b7gbkKD2ldeKf/BY0eiDYlpOfgyy5kolUFcGG/hq7bjcoIOjDDV0Y2\n\
uOMT7ZEa6Z0dB38p/5/hZ64fpKlALtZPZv+9MEiMOQ/tOzlCc3abwGa/fizFU0Fu\n\
V9jhAgMBAAGjYDBeMB8GA1UdIwQYMBaAFLY7Jui/k9bOUXxBJ6hVQS/9q7qNMB0G\n\
A1UdDgQWBBS7wrwCmlYcd5ArUbtsYZTEJ5/uPjAMBgNVHRMBAf8EAjAAMA4GA1Ud\n\
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEATyoTgVby8zyL8rgxUB6VE8as\n\
4f+3lrNUna3u88tdUs+GeA1gZ2byY5hlZRbSAoNflJd3qiZEtaZHf3CWkMzl9Vck\n\
uf3X8NOKCId79cq/EzQD5UtpFoGbDZexXg29U5OT2p7iVQsg32LvMvFJXGZLTVDM\n\
ocCi5oowSRJrMEeP6tASfzpm2YcndEENw9DCvWV+J5ZuzCec/xizWtVvVAw8L+ij\n\
hUXOhSnLysYMaz3wRQi32LI2vJlujd5iXz0e9DIoK+F+GhJ03tEwccsQ/5rYR7Tf\n\
DiThTx8YWIAzfqa88C+sL8tOCCVgykeei9bgDgHir6/uDPxzmlemAhWdTueypw==\n\
-----END CERTIFICATE-----\n";

// "The private key of the thing". The public key is not needed here
char private_pem_key[] = "-----BEGIN RSA PRIVATE KEY-----\n\
MIIEowIBAAKCAQEAxxxJUahmEgL74iNt30INcDbn/0sFl7NxCg2S1eSYa44Vq8YK\n\
xjk4QFSpZs6zcUaseS3MtYraKh6BLJDGOLQhhlI9HT+oEwJ+jAQwjrnRR5UOjaWG\n\
TZGDCiB3wVDyuoJcbPBGBBp97VaPbwpcnjkSIevM9RDh93C6gvfUwvlzkowUvk5s\n\
W0TZ6C/3tkFPYQ8HLf4G+6XCKzCwfSPgmu6P8chIxrOm+4G5Cg9pXXin/wWNHog2\n\
JaTn4MsuZKJVBXBhv4au243KCDoww1dGNrjjE+2RGumdHQd/Kf+f4WeuH6SpQC7W\n\
T2b/vTBIjDkP7Ts5QnN2m8Bmv34sxVNBblfY4QIDAQABAoIBACBLoEDUdanaPgzR\n\
/lJGHnPjFAI+ouuWOXGN/hEfgPGBHFa+SqUYRt14XfnE8Sogcn09RoTzvqsyQ3V9\n\
DgnIfd+Rm0QRPSvILcp8K3wa+60GqlOjU4NlZMHRvlrmzFggKOmql1C3E1HZxm4W\n\
Wit3yk8iNh5R3LUL3dS8ahAt8ecE9GHhPY2bBaG5MMeL/SpyY4FjKO3hnXmwb+pS\n\
8BJ0v/+/XGSOx1KQ1n0pC6A+5bfy7y4EV5voyU4xwoqQOb0OqubCL8fD3hWMQl4Z\n\
iU0RlQv+6FyPFFrCzy+LWtKB5OhTCm50Ngmklqb/OGr2uuAHQFcfcUCZafuJ6FDq\n\
Vbx5AJkCgYEA5BU9NGPRK3TYrg2oD4TyfgF6XEo9jDsU5zoxvwrNPFoWHEqNO3nU\n\
Kj0YMSve9EZkATXM4EWsHSXycQBs9wSmGTBXtRo3lAYGPrWKIOEIccvssDhVOy67\n\
h5eVn5boGXkxzBbaEqASvn3OAiPFbACMPpl7eMDO0pI58qaiPv84QV8CgYEA33s5\n\
BFxkn736UOnw2HAWOjAQA3w4G9lUDZcSDiq4y9maFm1VxQOTno0DVppxubi2Lq+S\n\
HV7g67MdT1NIrKBmrZOKVSy0z9C6haw4YHesP9NMomEHF0curwasOE+WLzVPG1r5\n\
bj4f/PVz6u/p8FhUnJVrMdMsR1RtgifgGYi6zb8CgYBIyeptdMmFN9YKvfr7cO8+\n\
fVRCbcL0pMpH/M9xa44SC6Hwx24JJBksnbDByIFvraMuq6/Fhxaler3DiGI99vLX\n\
KRFkj4hbKg0GKEN7uLVMM8tGdcPljkV0wVCTzMInlwDoZAaWWRWCS33aOvY/QHGB\n\
bNP1tnqKwvXRVZKDzklyuwKBgHt3SV3WaaUzQQrY5D2/VxhYm1BBnYINRHmpn8kB\n\
40yD8LQ5PWx1+GVdBJ4g/3maydPrHa9qTpsJPsUnfvwYNgHphEBgTZiOea4GIrNx\n\
ClHMvAbt0gwTQIBkEFuf5Jo6OnUjgrBcVDHdrWf7meQ1+LD1f95sC4dw5ntE6nNQ\n\
7dtjAoGBAJbjqX0Oy4JlEy3j0vNriib9acKz8Ool0R17rsmiPMXdDzhv2dwjdgIC\n\
08KP9yS68+emvu7/r1DMA2mOow0eKKo7oJcLIDEegH/FJgdKio6A5JtfBARG6UbW\n\
HN7SaLzWWlbfr9l+Jyfg7JWzbEzhkQg8waUMdKJB0PFqo+F/F5lu\n\
-----END RSA PRIVATE KEY-----\n";

// BG96 is the object of class _BG96_MQTT
// DSerial USB serial
// APN name
// APIN login,
// APN password ,
// MQTT server FQDN,
// MQTT port, number
// MQTT Client Id such as "BasibPubSub",
// MQTT topic name,
// MQTT QoS  such as AT_MOST_ONCE,
// MQTT index: The range is 0 ~ 5
// PDP index:  The range is 1 ~ 16
// SSL index:  The range is 0 ~ 5

// char *ModemIMEI  is an output contains the IMEI
bool InitModemMQTT(_BG96_MQTT &BG96,
                   Stream &DSerial,
                   char *APN,
                   char *LOGIN,
                   char *PASSWORD,
                   char *MQTTServer,
                   unsigned int MQTTPort,
                   char *MQTTClientId,
                   char *mqtt_topicName,
                   Mqtt_Qos_t MQTT_QoS = AT_MOST_ONCE,
                   unsigned int MQTTIndex = 0,
                   unsigned int PDPIndex = 1,
                   unsigned int SSLIndex = 2,
                   char *ModemIMEI = NULL)
{
  Mqtt_Version_t version = MQTT_V4;

  char imei_tmp[64];
  if (BG96.GetDevIMEI(imei_tmp))
  {
    String s = String(imei_tmp);
    s.trim();
    s.toCharArray(ModemIMEI, 64);
    DSerial.println(ModemIMEI);
  }

  BG96.DeleteFiles("*");

  char apn_error[64];
  while (!BG96.InitAPN(PDPIndex, APN, LOGIN, PASSWORD, apn_error))
  {
    DSerial.println(apn_error);
  }
  DSerial.println(apn_error);

  char ssl_error[128];
  while (!BG96.InitSSL(SSLIndex, aws_root_ca_pem, certificate_pem_crt, private_pem_key, ssl_error))
  {
    DSerial.println(ssl_error);
  }
  DSerial.println(ssl_error);

  DSerial.println("\r\nStart Config the MQTT Parameter!");
  while (!BG96.SetMQTTConfigureParameters(MQTTIndex, PDPIndex, version, 150, SERVER_STORE_SUBSCRIPTIONS))
  {
    DSerial.println("\r\nConfig the MQTT Parameter Fail!");
    int e_code;
    if (BG96.returnErrorCode(e_code))
    {
      DSerial.print("\r\nERROR CODE: ");
      DSerial.println(e_code);
      DSerial.println("Please check the documentation for error details.");
    }
  }
  DSerial.println("\r\nConfig the MQTT Parameter Success!");

  while (!BG96.SetMQTTEnableSSL(MQTTIndex, SSLIndex, true))
  {
    DSerial.println("\r\nSetMQTTEnableSSL the MQTT Parameter Fail!");
    int e_code;
    if (BG96.returnErrorCode(e_code))
    {
      DSerial.print("\r\nERROR CODE: ");
      DSerial.println(e_code);
      DSerial.println("Please check the documentation for error details.");
    }
  }
  DSerial.println("\r\nSetMQTTEnableSSL the MQTT Parameter Success!");


  while (BG96.OpenMQTTNetwork(MQTTIndex, MQTTServer, MQTTPort) != 0)
  {
    DSerial.println("\r\nSet the MQTT Service Address Fail!");
    int e_code;
    if (BG96.returnErrorCode(e_code))
    {
      DSerial.print("\r\nERROR CODE: ");
      DSerial.println(e_code);
      DSerial.println("Please check the documentation for error details.");
    }
  }
  DSerial.println("\r\nSet the MQTT Service Address Success!");

  DSerial.println("\r\nConfigure Timeout!");
  while (!BG96.SetMQTTMessageTimeout(MQTTIndex, 10, 5, 1))
  {
    DSerial.println("\r\nMQTT Timeout Fail!");
    int e_code;
    if (BG96.returnErrorCode(e_code))
    {
      DSerial.print("\r\nERROR CODE: ");
      DSerial.println(e_code);
      DSerial.println("Please check the documentation for error details.");
    }
  }

  DSerial.println("\r\nStart Create a MQTT Client!");
  while (BG96.CreateMQTTClient(MQTTIndex, MQTTClientId, "", "") != 0)
  {
    DSerial.println("\r\nCreate a MQTT Client Fail!");
    int e_code;
    if (BG96.returnErrorCode(e_code))
    {
      DSerial.print("\r\nERROR CODE: ");
      DSerial.println(e_code);
      DSerial.println("Please check the documentation for error details.");
    }
  }
  DSerial.println("\r\nCreate a MQTT Client Success!");

  DSerial.println("\r\nStart MQTT Subscribe Topic!");
  while (BG96.MQTTSubscribeTopic(MQTTIndex, 1, mqtt_topicName, MQTT_QoS) != 0)
  {
    DSerial.println("\r\nMQTT Subscribe Topic Fail!");
    int e_code;
    if (BG96.returnErrorCode(e_code))
    {
      DSerial.print("\r\nERROR CODE: ");
      DSerial.println(e_code);
      DSerial.println("Please check the documentation for error details.");
    }
  }
  DSerial.println("\r\nMQTT Subscribe Topic Success!");

  return true;
}

#endif
