#ifndef __GNSS_H_
#define __GNSS_H_

#include "MQTT_AWS.h" // GNSS erbt von MQTT_AWS
#include "GNSS_Position.cpp"

class GNSS : public MQTT_AWS
{
protected:
    // GNSS-Tracker-Struktur
    struct GNSS_Data
    {
        char gsa[128];
        char gsv[516];
        GNSS_Work_Mode_t workMode = MS_BASED;
        unsigned int geoID = 1;
    } gnssData;
    typedef enum powersaving_strategy
    {
        POWERSAVING_DISABLE = 0,
        DISTANT_BASED = 1,
        DISTANT_BASED_WITH_SPEED = 2,
        DISTANT_BASED_WITH_MOTION = 3,
        PERIOD_WITH_MOTION = 4
    } Powersaving_Strategy;

    bool getFirstFix = false;
    bool gpsModuleEnable = false;

public:
    // Konstruktor
    GNSS(Stream &atSerial, Stream &dSerial, JsonDocument &docInput);

    // Methoden
    bool InitGNSS();                 // Initialisiert GNSS
    void GPSOneXtraCheckForUpdate(); // Prüft auf GPSOneXTRA-Updates
    bool handleGNSSMode(Powersaving_Strategy gnss_strategy);           // Handhabt GNSS-Modus
    // bool addGeo();
    /// @brief handlePowersavingMode GNSS
    /// @return true when getGNSSFix and should send this to Server, false when not
    bool handlePeriod();
    bool handleDistantBased();
    bool handleDistantBasedWithSpeed();
    bool handleDistantBasedWithMotion();
    bool updatePosition();
    bool TurnOff();
    bool PowerDownGNSS();
    bool TurnOnGNSS();
    bool isGnssModuleEnable();
    bool GetGnssPositionAndError(GNSS_Position &current_fix);
    bool updatePositionAndResetSpeed();
    bool turnOffModem();

private:
    unsigned long currentTime;
    unsigned long TTFF = 0;
    // unsigned int fixCount = 0;
    GNSS_Position *last_position = nullptr;
    unsigned long gnss_start_time = 0;
    int max_distant = 50;
    float distant_from_last = 0;
    float assumed_speed = 2;
    float max_speed = 0.8333; ///durchschnittliche Gehtempo
    unsigned long time = 0;
};

#endif // __GNSS_H_
