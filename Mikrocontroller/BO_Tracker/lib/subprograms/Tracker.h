#ifndef __TRACKER_H_
#define __TRACKER_H_

#include "GNSS.h"

class Tracker : public GNSS {

public:
    // Konstruktor
    Tracker(Stream &atSerial, Stream &dSerial, JsonDocument &doc);

    ~Tracker();
    // Methode zum Initialisieren des Trackers
    bool InitModule();

    void firstStart();

    bool modeHandle();

    bool pubAndsubMQTT(unsigned long interval);

    bool sendAndWaitResponseHTTP(unsigned long interval);

    bool responseValid(char *payload);

    int getInitErrorCount();

    int getRunningErrorCount();

    int checkForError();

    bool turnOnFunctionality(bool useMQTT);

    bool wakeUp();

    bool handleCellInfosMode();

    bool handleErrors();

    bool setMode(char *payload);

    bool pingServer();

    bool resetModem();

private:
 
    long measure_time = -99999;
    int countReset = 0;
  
};

#endif  // __TRACKER_H_
