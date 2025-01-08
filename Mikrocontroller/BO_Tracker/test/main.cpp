
#include <Tracker.h>
#define DSerial SerialUSB
#define ATSerial Serial1

JsonDocument docOutput;

Tracker tracker(ATSerial, DSerial, docOutput);

void setup(void)
{
    Serial.begin(115200);
    Serial.println("BMA456 Step Counter");

    tracker.initialize(RANGE_4G, ODR_1600_HZ, NORMAL_AVG4, CONTINUOUS);
    tracker.enableWakeOnMotion();
    if (tracker.setcurrentTime("25/01/07,03:02:09+04") && tracker.enableAlarm(100000))
    {
        DSerial.println("Set successful");
    }
}

void loop(void)
{
    // if(tracker.waitForMotion()){
    //     DSerial.println("Motion Triggern");
    // }
    // if(tracker.checkForMotionInMillis(1000,100)){
    //     DSerial.println("Motion above Threshold Triggern");
    // }
    // delay(1000);

    if (trackerModes.wakeUp == true)
    {
        DSerial.println("Alarm Triggern");
        trackerModes.wakeUp = false;
    }
    delay(200);
}
