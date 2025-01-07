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

    bool sendAndCheck();

    bool pubAndsubMQTT();

    bool sendAndWaitResponseHTTP();

    bool responseValid(char *payload);

    bool turnOffTracker();

    int getInitErrorCount();

    int getRunningErrorCount();

    int checkForError();


private:
    unsigned long pub_time = 0;
    bool modeRequest = true;

};

#endif  // __TRACKER_H_
