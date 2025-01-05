#ifndef __GNSS_H_
#define __GNSS_H_

#include "MQTT_AWS.h"  // GNSS erbt von MQTT_AWS

class GNSS : public MQTT_AWS {
protected:
    // GNSS-Tracker-Struktur
    struct GNSS_Tracker {
        char gsa[128];
        char gsv[516];
        unsigned long startMillis = 0;
        GNSS_Work_Mode_t workMode = MS_BASED;
        bool isOn = false;
        unsigned int geoID = 1;
    } gnssTracker;

public:
    // Konstruktor
    GNSS(Stream &serial, _BG96_Module &gnssModule, JsonDocument &doc);

    // Methoden
    bool InitGNSS();               // Initialisiert GNSS
    void GPSOneXtraCheckForUpdate();  // Prüft auf GPSOneXTRA-Updates
    void handleGNSSMode();          // Handhabt GNSS-Modus
    bool addGeo();                  // Fügt Geofencing hinzu
};

#endif  // __GNSS_H_
