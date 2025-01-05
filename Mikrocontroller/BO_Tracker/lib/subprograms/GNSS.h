#ifndef __GNSS
#define __GNSS

// #include "MQTT_AWS.hpp"
#include "BG96_GNSS.h"
#include "TrackerMode.cpp"
#include "Network_Infos.h"
class GNSS {
private:
    // Private Attribute
    Stream &DSerial;
    _BG96_GNSS &_GNSS;
    JsonDocument &docInput;

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
    GNSS(Stream &serial, _BG96_GNSS &gnssModule, JsonDocument &doc);

    // Methoden
    bool InitGNSS();
    void GPSOneXtraCheckForUpdate();
    void handleGNSSMode();
    bool addGeo();
};

#endif
