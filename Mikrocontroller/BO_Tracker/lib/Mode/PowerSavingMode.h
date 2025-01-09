#ifndef __POWERSAVINGMODE_H_
#define __POWERSAVINGMODE   _H_

#include "Tracker.h"

class PowerSavingMode {
private:
    
    Tracker &tracker;

public:
    
    PowerSavingMode(Tracker &trackerObj);
    void start();
    bool setup();
    bool sendData();
};

#endif 