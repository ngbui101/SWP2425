// TrackerMode.h
#define TRACKER_MODE_H_

struct TrackerModes {
    // Mode
    bool GnssMode = false;
    bool CellInfosMode = true;
    bool BatteryMode = true;
    bool TemperatureMode = true;
    bool NmeaMode = false;
    bool GeoFenMode = false;
    // bool RequestMode = true;
    // bool Modem_Off = false;
    unsigned long period = 20000UL;
    unsigned long maxRealTime = 120001UL;
    bool realtime = true;
    // GeoFencing
    float geoLatitude = 0;
    float geoLongitude = 0;
    unsigned int geoRadius = 0;

    bool wakeUp = true;

    // void resetModes() {
    //     GnssMode = false;
    //     CellInfosMode = true;
    //     BatteryMode = true;
    //     TemperatureMode = true;
    //     NmeaMode = false;
    //     GeoFenMode = false;
    //     // RequestMode = true;
    //     period = 60000UL;
    //     geoLatitude = 0;
    //     geoLongitude = 0;
    //     geoRadius = 0;
    // }

    // void updateRequestMode(){
    //     if(GnssMode || CellInfosMode || TemperatureMode || NmeaMode || BatteryMode){
    //         RequestMode = false;
    //     } else {
    //         RequestMode = true;
    //     }
    // }
};

extern TrackerModes trackerModes;
