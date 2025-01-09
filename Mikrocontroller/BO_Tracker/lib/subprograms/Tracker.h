#ifndef __TRACKER_H_
#define __TRACKER_H_

#include "MQTT_AWS.h"

class Tracker : public MQTT_AWS {

public:
    // Konstruktor
    Tracker(Stream &atSerial, Stream &dSerial, JsonDocument &doc);

    ~Tracker();
    // Methode zum Initialisieren des Trackers
    bool InitModule();

    bool setCurrentTimeToRTC();  

    void firstStart();

    bool setMode(char *payload);

    bool modeHandle();

    bool sendAndCheck();

    bool pubAndsubMQTT();

    bool sendAndWaitResponseHTTP();

    bool responseValid(char *payload);

    int getInitErrorCount();

    int getRunningErrorCount();

    int checkForError();

    bool turnOffModem();

    bool resetModem();

    bool turnOnFunctionality();

    bool wakeUp();

    bool handleCellInfosMode();

    bool retryIn1Hour();

    int getResetCount();

    bool handleErrors();
    
    bool handleIniTErrors();
private:
    unsigned long pub_time = 0;

    int countReset = 0;

    // bool init = false;
    // bool modeRequest = true;
};

#endif  // __TRACKER_H_
