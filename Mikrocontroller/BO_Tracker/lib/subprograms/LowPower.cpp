#include "Lowpower.h"

_Lowpower::_Lowpower() : _Motion() {
}

_Lowpower::~_Lowpower() {
}

void _Lowpower::deepSleep(unsigned long millis)
{   
    millis = (millis <= minSleepTime) ? minSleepTime : maxSleepTime;
    
    LowPower.deepSleep(millis);
}

bool _Lowpower::deepSleepWithAlarm(unsigned long millis){
    if((countMaxSleep == 0) && (countMinSleep == 0)){
        countMaxSleep = millis / maxSleepTime;
        countMinSleep = (millis % maxSleepTime) / minSleepTime;

        // Serial.print("Go to Sleep for: ");
        // Serial.print(countMaxSleep);
        // Serial.print("x 2 Minutes and ");
        // Serial.print(countMinSleep);
        // Serial.println("x 10 Seconds");

        trackerModes.wakeUp = true;
        
        return true;
    }else if(countMaxSleep != 0){
        
        // Serial.print("Max Sleep: ");
        // Serial.println(countMaxSleep);

        countMaxSleep--;
        LowPower.deepSleep(maxSleepTime);
        return true;
    }else if(countMinSleep != 0){

        // Serial.print("Min Sleep: ");
        // Serial.println(countMinSleep);

        countMinSleep--;
        LowPower.deepSleep(minSleepTime);
        return true;
    }
    return true;
}