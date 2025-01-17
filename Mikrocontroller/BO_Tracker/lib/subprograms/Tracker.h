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

    void firstStart();

    bool modeHandle();

    bool sendAndCheck();

    bool pubAndsubMQTT(unsigned long interval);

    bool sendAndWaitResponseHTTP(unsigned long interval);

    bool responseValid(char *payload);

    int getInitErrorCount();

    int getRunningErrorCount();

    int checkForError();

    bool turnOffModem();

    bool resetModem();

    bool turnOnFunctionality(bool useMQTT);

    bool wakeUp();

    bool handleCellInfosMode();

    bool retryIn1Hour();

    int getResetCount();

    bool handleErrors();
    
    bool handleIniTErrors();
private:
    unsigned long pub_time = 0;

    unsigned long start_time = 0;

    unsigned long measure_time = -99999;

    unsigned long post_time = 9999999;
    int countReset = 0;
    // bool cellscanning = false;
    // bool init = false;
    // bool modeRequest = true;
};

#endif  // __TRACKER_H_
