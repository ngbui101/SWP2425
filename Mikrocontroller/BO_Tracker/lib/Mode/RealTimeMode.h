#ifndef __REALTIMEMODE_H_
#define __REALTIMEMODE_H_

#include "Tracker.h"

class RealTimeMode {
private:
    // Ein Tracker-Objekt als Attribut
    Tracker &tracker;

public:
    // Konstruktor
    RealTimeMode(Tracker &trackerObj);

    // Methoden
    void start();
    bool setup();
    void loop();
};

#endif // __REALTIMEMODE_H_
