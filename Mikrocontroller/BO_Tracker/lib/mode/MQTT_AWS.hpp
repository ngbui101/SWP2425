
#ifndef __MQTT_AWS_H
#define __MQTT_AWS_H

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
MIIDWjCCAkKgAwIBAgIVAPSEr1W2DcQ3C/9HbHbIdtOJTZ9uMA0GCSqGSIb3DQEB\n\
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n\
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yNDEwMjkxNjAy\n\
NTZaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n\
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDD3k8jLePOQhvhfuSw\n\
S/2P+MCrYAAAtxfD61XWAXDJCbOWXWo+CrKrJ1QVMIkLCOtDSEhl5FdJWgBvX9Ic\n\
HcfsKmjUOz11QmMoYpDknHP08tjGHVPiJmydzS+gBYdi7sW4OAanOXSWdwfNscW8\n\
ScxnvQXK+fzeIGFLdKIeotZ2HBvy7Sx94fyBeH56APQlBnY1dIEVhp/EKSbn2BXT\n\
0N+Q450sLiHlEaDWxb5uXKULo70CWnQgAwLozh4zhjUCJb8R3DCCjc4KzWmUZ0/7\n\
GpRwcQj/jXEeC3WBeOg/RtfT7MtDgmdsvgWpyR2psTy3eZlBhufpJa2enBy/rVzj\n\
G/25AgMBAAGjYDBeMB8GA1UdIwQYMBaAFDzVGCrivEnTPsSmUHw/kYGOOVt5MB0G\n\
A1UdDgQWBBS/OHj0u/pYwo+mVwWEoTO729MxDDAMBgNVHRMBAf8EAjAAMA4GA1Ud\n\
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEApumUlGXEUcMt7ERomiEM3HpJ\n\
DBNitfAMLQd8Lc1SaCdgcaDDekRlyglQN4hU75Ka+GoDnSZY0fgdZVQvLxJ3OfPd\n\
Bj1AR7M6XUEhmdonc6Tg0APLU1NATP1cdLa1dC3rsI1C+uhk9SpfMRfkTZWY4ar9\n\
UDEYn+xtIUkQSg3alRoZkquft3uRGZBI5+3UahuN8oW1mqpXZNkYrqWt9m6I3QOZ\n\
nm0NpoBIUayAIAk1kJw/HABLmJ81BAKpqWH2O8d6OQ2M5PGj6BcrMJrCPLYWeU6N\n\
8T+iIRkussJwJJPQ7xnrF/bqxuGJGGIk64uC2byG9tt6XEcv9HJELhAs1L2A7A==\n\
-----END CERTIFICATE-----\n";

// "The private key of the thing".
char private_pem_key[] = "-----BEGIN RSA PRIVATE KEY-----\n\
MIIEpQIBAAKCAQEAw95PIy3jzkIb4X7ksEv9j/jAq2AAALcXw+tV1gFwyQmzll1q\n\
PgqyqydUFTCJCwjrQ0hIZeRXSVoAb1/SHB3H7Cpo1Ds9dUJjKGKQ5Jxz9PLYxh1T\n\
4iZsnc0voAWHYu7FuDgGpzl0lncHzbHFvEnMZ70Fyvn83iBhS3SiHqLWdhwb8u0s\n\
feH8gXh+egD0JQZ2NXSBFYafxCkm59gV09DfkOOdLC4h5RGg1sW+blylC6O9Alp0\n\
IAMC6M4eM4Y1AiW/Edwwgo3OCs1plGdP+xqUcHEI/41xHgt1gXjoP0bX0+zLQ4Jn\n\
bL4FqckdqbE8t3mZQYbn6SWtnpwcv61c4xv9uQIDAQABAoIBAQCIogrIepmgZ2N1\n\
oQQ8qzXdeeRFaDl4RkDQzdjZ6bVYuGjLqqyTTPkoBc5U3b+rr/QVwlWmpmvxYlRN\n\
XSV/sZKl/0uTSSPtKXtsT53NrAoDxGhNg2Xhq3Y3ekwPr+uqqu/IO2UAwEbBM5WA\n\
+tL6LuzlLOnelxa7iRD2Bw2Um3lJd8fUhgFSUxywVgqfY943jNT/tkoMowcl7a7W\n\
tgzPQVFoduYut7T75tHt6l4U4k/ewUm+ySx1ju9amRrmw+fOpVfRcMfBAkHVmF9w\n\
bGOUHwB2+J9Cy0uQ5VHuaU4hUyIcTCIVr0wnvBzopvVygFnRwItAqPTLSkDzCbAe\n\
VGxB3d6RAoGBAOUdIP68sB23UTsC5oHj0K6eQvQ9xnUgeHuDrTFyGdXlyNcTiI9N\n\
+W/DjsNtousEjMlG0GjPzLREDUT0x2KaTNOon+NIeA2ftRCQRoAunANPYGZeq8oO\n\
RmwR0k62wX4tBRSyrz+PE9aqdG9g5rcoh1OLk7BRryi4vxrqMqYXAa8NAoGBANra\n\
cjttXYfiyx2afY6ZEB/ZeacfV4sn0LH/JnFspoJqTEQBx0GcKc24vUFCrRnN1Qkc\n\
l9ckicJxE7cdqHWPYVG8KcydatszzvVSHw2sGkjnoPLXYrB2Xg9YZN+XwZEMDPF1\n\
XKaeqVK/Of4ChDMEyhKAKgHiRMakV/gpxhIy835dAoGBAKkT8gGLZbzIR93wqgM5\n\
CAuVJ3vvCsHtEHy2E93lgkNjq+9srt5ygJ2GB5kZE520YeE3rR1sHHj7r38v1ORj\n\
Ukbwre2g0VfcgFaU4jI+M0wOC+n/+27FUhr2m+kS3YAr4RABPWr+P3mvmPWI4TpR\n\
Ymy5GGiKH2VB0WYfZvlDCS8RAoGBAJ4Suf+CdtP3U44GUDn3BFZ81gmH8cto7qwi\n\
BrpWJ856yKs2PZ1+FhOpI8lHR5Ajw4PZSzLyCS6QThUtLsf6z+ghaG+KI+w/93Pp\n\
5NQTQsj1xKKiP34FgNrEldeJ3R3O/mpIwqr2XoaN6G03lBTlmK7UM4M400RG3+M3\n\
CQGMG6NhAoGAezE/QTa9TyZZCwPsH1lNfyYiDrgcU9hpIo8pp4TOdMxE4109XKqJ\n\
gP5HpbKQB0mBEADXAwWVXj2i8UL2nW9lo0xObxbqVSZJE8Xj+zKJCksacTq+we4/\n\
f1ekEo4pJ+I1iQkqLLspRGJQziDcstLWuH+62b/f9wS9P3sKrcTb5PE=\n\
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
                   char *mqtt_sub_topic,
                   char *mqtt_pub_topic,
                   Mqtt_Qos_t MQTT_QoS = AT_MOST_ONCE,
                   unsigned int MQTTIndex = 0,
                   unsigned int PDPIndex = 1,
                   unsigned int SSLIndex = 2,
                   char *ModemIMEI = NULL)
{
  Mqtt_Version_t version = MQTT_V4;

  BG96.ConfigNetworks();
  BG96.SetDevCommandEcho(false); 
  BG96.SetDevOutputformat(true);
  // IMEI
  char imei_tmp[64];

  if (BG96.GetDevIMEI(imei_tmp))
  {
    String s = String(imei_tmp);
    s.trim();
    s.toCharArray(ModemIMEI, 64);
    DSerial.println(ModemIMEI);
  }
  // MQTT Subscribe Topic zusammensetzen mit strcat
  strcpy(mqtt_sub_topic, "tracker/"); // Basisteil des Topics
  strcat(mqtt_sub_topic, ModemIMEI);  // IMEI anhängen
  strcat(mqtt_sub_topic, "/sub");     // Suffix "/sub" anhängen

  // MQTT Publish Topic zusammensetzen mit strcat
  strcpy(mqtt_pub_topic, "tracker/"); // Basisteil des Topics
  strcat(mqtt_pub_topic, ModemIMEI);  // IMEI anhängen
  strcat(mqtt_pub_topic, "/pub");     // Suffix "/pub" anhängen

  // SSL Networking
  BG96.DeleteCertificate("all");

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

  // MQTT
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
  while (BG96.MQTTSubscribeTopic(MQTTIndex, 1, mqtt_sub_topic, MQTT_QoS) != 0)
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
  BG96.DeleteCertificate("all");
  return true;
}

#endif
