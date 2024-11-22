struct TrackerModes {
    bool GnssMode = false;
    bool CellInfosMode = false;
    bool BatteryMode = false;
    bool TemperatureMode = false;
    bool NmeaMode = false;
    bool GeoFenMode = false;
    bool RequestMode = true;
    bool Modem_Off = false;
    unsigned int frequenz = 5000UL;

    float geoLatitude = 0;
    float geoLongitude = 0;
    unsigned int geoRadius = 0;

    // Methode zum Zurücksetzen aller Modi auf Standardwerte
    void resetModes() {
        GnssMode = false;
        CellInfosMode = false;
        BatteryMode = false;
        TemperatureMode = false;
        NmeaMode = false;
        GeoFenMode = false;
        RequestMode = true;
        frequenz = 5000UL;
        geoLatitude = 0;
        geoLongitude = 0;
        geoRadius = 0;
    }
    void updateRequestMode(){
        if(GnssMode||CellInfosMode||TemperatureMode||NmeaMode||BatteryMode){
            RequestMode = false;
        }else{
            RequestMode = true;
        }
    }
};

TrackerModes trackerModes;
