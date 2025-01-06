// TrackerMode.h
#ifndef TRACKER_MODE_H_
#define TRACKER_MODE_H_
#include "Cell.h"
struct TrackerModes {
    // Mode
    bool GnssMode = false;
    bool CellInfosMode = false;
    bool BatteryMode = false;
    bool TemperatureMode = false;
    bool NmeaMode = false;
    bool GeoFenMode = false;
    bool RequestMode = true;
    bool Modem_Off = false;
    unsigned int period = 5000UL;

    char modemIMEI[20]; // IMEI des Modems
    Cell *cells[6] = {nullptr}; // Zellinformationen
    // GeoFencing
    float geoLatitude = 0;
    float geoLongitude = 0;
    unsigned int geoRadius = 0;

    void resetModes() {
        GnssMode = false;
        CellInfosMode = false;
        BatteryMode = false;
        TemperatureMode = false;
        NmeaMode = false;
        GeoFenMode = false;
        RequestMode = true;
        period = 5000UL;
        geoLatitude = 0;
        geoLongitude = 0;
        geoRadius = 0;
    }

    void updateRequestMode(){
        if(GnssMode || CellInfosMode || TemperatureMode || NmeaMode || BatteryMode){
            RequestMode = false;
        } else {
            RequestMode = true;
        }
    }
};

extern TrackerModes trackerModes;

#endif // TRACKER_MODE_H_
