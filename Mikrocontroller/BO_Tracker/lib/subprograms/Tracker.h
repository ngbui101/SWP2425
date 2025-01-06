#ifndef __TRACKER_H_
#define __TRACKER_H_

#include "MQTT_AWS.h"

class Tracker : public MQTT_AWS {

public:
    // Konstruktor
    Tracker(Stream &atSerial, Stream &dSerial, JsonDocument &doc);

    // Methode zum Initialisieren des Trackers
    void InitModule();

    bool setCurrentTimeToRTC();  

    void setModeRequest(bool modeRequest);

    void firstStart();

    bool setMode(char *payload);

    bool modeHandle();
private:
    
    bool modeRequest = true;
    Cell *cells[6] = {nullptr};

};

#endif  // __TRACKER_H_
