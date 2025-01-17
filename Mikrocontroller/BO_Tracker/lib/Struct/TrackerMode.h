// TrackerMode.h
#define TRACKER_MODE_H_

struct TrackerModes
{
    // Mode
    bool GnssMode = false;
    bool CellInfosMode = false;
    bool BatteryMode = false;
    bool TemperatureMode = false;
    bool NmeaMode = false;
    bool GeoFenMode = false;
    unsigned long period = 60000UL;
    unsigned long maxRealTime = 120001UL;
    bool realtime = false;
    char RAT[8] = "lte";
    // char  RAT[8] = "gsm";
    // char RAT[8] = "nbiot";

    // GeoFencing

    bool wakeUp = false;
};

extern TrackerModes trackerModes;
