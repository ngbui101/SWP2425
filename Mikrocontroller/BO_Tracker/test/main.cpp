
#include <Tracker.h>
#define DSerial SerialUSB
#define ATSerial Serial1


JsonDocument docOutput;

Tracker tracker(ATSerial, DSerial,docOutput);

void setup(void) {
    Serial.begin(115200);
    Serial.println("BMA456 Step Counter");

    tracker.initialize(RANGE_4G, ODR_1600_HZ, NORMAL_AVG4, CONTINUOUS);
    tracker.enableWakeOnMotion();
}

void loop(void) {
    if(tracker.waitForMotion()){
        DSerial.println("Motion Triggern");
    }
    if(tracker.checkForMotionInMillis(1000,100)){
        DSerial.println("Motion above Threshold Triggern");
    }
    delay(1000);
}
