#ifndef __TRACKER_H_
#define __TRACKER_H_

#include "GNSS.h"

class Tracker : public GNSS {

public:
    // Konstruktor
    Tracker(Stream &atSerial, Stream &dSerial, JsonDocument &doc);

    // Methode zum Initialisieren des Trackers
    void InitModule();

    bool setRTC();  

};

#endif  // __TRACKER_H_
