#ifndef __GNSS_H_
#define __GNSS_H_

#include "HTTP.h"  // GNSS erbt von MQTT_AWS

class GNSS : public HTTP {
protected:
    // GNSS-Tracker-Struktur
    struct GNSS_Data {
        char gsa[128];
        char gsv[516];
        unsigned long startMillis = 0;
        GNSS_Work_Mode_t workMode = MS_BASED;
        unsigned int geoID = 1;
    } gnssData;
    JsonDocument &docInput;

    bool getFirstFix = false;
    bool gpsModuleEnable = false;
public:
    // Konstruktor
    GNSS(Stream &atSerial, Stream &dSerial, JsonDocument &docInput);

    // Methoden
    bool InitGNSS();               // Initialisiert GNSS
    void GPSOneXtraCheckForUpdate();  // Prüft auf GPSOneXTRA-Updates
    void handleGNSSMode();          // Handhabt GNSS-Modus
    bool addGeo();
    
    bool TurnOff();
    bool TurnOnGNSS();
    bool isGnssModuleEnable();
};

#endif  // __GNSS_H_
