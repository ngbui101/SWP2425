
#ifndef __BG96_AWS_H
#define __BG96_AWS_H

#include "BG96_MQTT.h"

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
MIIDWTCCAkGgAwIBAgIUcbPitpZgsEj8wqYTF1nCfxG+vt8wDQYJKoZIhvcNAQEL\n\
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n\
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI0MTAwNDA5MzYw\n\
N1oXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n\
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMpQF3AMS1SavzeGun78\n\
xIdWWrfKTbQxlxUUWHTW7REGeWtAesQtTDfIhIjniizSa8TKmEd2MO2joFO7D0bU\n\
bicmh+YBmQSPtk1LOM+W5YOgtiTALYRG+LjepgACG6fPosXLRTKZXz3Zcuc1sxwo\n\
48+TOtVUKXcq5PAcsc8eeRBptkDKa2qB6+zzzB9OPp9a6YMGHaT+Rk5QxZY+clYT\n\
Ui7c8OgQSCVvqQO4+O0A1Ts02CEQQzUXndkwxIG8Hbgv0CiK0nzOY3aXsTqRG5P9\n\
9ieP53P/A9JbHEeb5Vqr6TuRArw4iJ1nfNhOS58nBXHIufnfeGnn/DYnsgv4vrO6\n\
Fw0CAwEAAaNgMF4wHwYDVR0jBBgwFoAUsY4iilQJO2MJJWsGds0vgd7MZ18wHQYD\n\
VR0OBBYEFI8MPRs0ZZ1sr6hi1V9Z1j7+K1L0MAwGA1UdEwEB/wQCMAAwDgYDVR0P\n\
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAW7NNywG/h/F0NDKnZTpCojiEY\n\
rTHvUB/FxSjEd3J97ZZydtFn/vI/X3lvEI4hqFAcjD15ykdegrNKJatkxwdR+ldX\n\
9JWFwE0eR243u+QQ7bbW7ZyWbcCe7HLGXiIdtfOiVCisoZ3DyIS7fowKOuJXHJZo\n\
7N/t4lY6TpAHdoci381cKzKQ798OENYGd+K0gxw2EXuYzeAUmVz2LoZWpZLCa1vu\n\
HWPRYqKDSzUDkAZERZktYDcMtI7Gt+cnxuSQnCplPNGygxg9kzh6w7FLF7gjGwQi\n\
I5rFRXQpcbV2AtWORxB3dvpf0pIFrA3L/yiO+DyRtNv3YDDxH2DJkwPEcvcd\n\
-----END CERTIFICATE-----\n";

// "The private key of the thing". 
char private_pem_key[] = "-----BEGIN RSA PRIVATE KEY-----\n\
MIIEpAIBAAKCAQEAylAXcAxLVJq/N4a6fvzEh1Zat8pNtDGXFRRYdNbtEQZ5a0B6\n\
xC1MN8iEiOeKLNJrxMqYR3Yw7aOgU7sPRtRuJyaH5gGZBI+2TUs4z5blg6C2JMAt\n\
hEb4uN6mAAIbp8+ixctFMplfPdly5zWzHCjjz5M61VQpdyrk8Byxzx55EGm2QMpr\n\
aoHr7PPMH04+n1rpgwYdpP5GTlDFlj5yVhNSLtzw6BBIJW+pA7j47QDVOzTYIRBD\n\
NRed2TDEgbwduC/QKIrSfM5jdpexOpEbk/32J4/nc/8D0lscR5vlWqvpO5ECvDiI\n\
nWd82E5LnycFcci5+d94aef8NieyC/i+s7oXDQIDAQABAoIBACPTHONdefVoVZXq\n\
iJm1/sMMWpITA8F1ypAUAznlWSNbpyKDz3lvoAB8xGNNxNz9AdJeIar91dou0FQj\n\
WqgrM7pBC22COOti22BCgrPXv5qYWciWMzkqLB7oZjK4LNeBYAxwWzeDiybzm3Iv\n\
wyCoaPa31oIyHgmZEeMm4aGQftWV4e+xEIZLf68DLrF/WwPXe0Glz/Y7QBdxDAY0\n\
GaIWOZfvRiayM/Au84qXHKZQleHymYR7nCj87XxN3VXcb6rDKLBsAiWE1swfcuCP\n\
UqAeuyr7oJkn0d0TavZ4WMYQK8VyXH4nbGln6aL36NzEU8RABwmeyq+g4WV8Q9em\n\
Ew3IAw0CgYEA6Cs0TisKr8Z3INl3lHkPj7JK3eckj3QKLxRwkqHavUhezkk/FoiO\n\
RpS6ByH1dPodd3PfdjlxaLKM2CEHFOgM4zxr+aHnv68G3qyLAcGMv+skjReNwUm+\n\
SRX52SXvrQWjp+1YFkeF/0tJtpN62JgmBMcCrKXQm4H+f/DHpl/auE8CgYEA3xRZ\n\
3XdR3eEGU5fxWGSUTJ72utq+YB8kbfpx0sXkzo9tTU5bx4U2ikLRhM9c50S4cg68\n\
F42GyNV50d5L9B3sAnCHLQASSdgPjdbhvie9AHYb/gZmvLODFKs3yCpkAELwd2hp\n\
k+nqKKsu5V8H8+yLif3yE5q3o5/DbZ/hmWw5h+MCgYAlnWpaB8sxc1w1rEYdfgex\n\
ZleO4DYiL9C+k/cxKpv/8su5bFCWXAWBFfOV2ECmdSd1BQb9Zo0jQ1piYed90aLU\n\
5iMwTLT5b2gTh6z12d/t8DD50wkaedIkkTTro6PTSoibWm7dHTVuiUomFHrgwLXY\n\
LE4YnrzPlR4gqGgQYpNTLQKBgQCy70gF9KeeKdlBRCc6l7K8ouWVi9AYmeJGtRSH\n\
1GRrq3ApCilkKN5OmLompY4Hk3+/PUJyQftTLKjCRmT2MuoUKt7CocQJp10SkMIS\n\
HR3lONfLMqdFxVfnRE9CJ35/FzJ7FS1aj85bQLQlF8zxQ1PKJAAA6aFThdrkG9KB\n\
leWvsQKBgQC1luw3t9QD8uT7XRYe6AD1BHdybqfLehXjrSukUZTpqkKvm9FhMiWL\n\
B7MAZWogP959JP1jijgmyXxdli07PXIQdHff1t2SE1wC2K5P+UduIhYvxGaaqJCr\n\
Bg7BK6gfp+ynmqe3+tGwCsZUVw2ARPGDg1xBxSQbzIlUkxt+/b4GHA==\n\
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
  BG96.DeleteFiles("*");
  return true;
}

#endif
