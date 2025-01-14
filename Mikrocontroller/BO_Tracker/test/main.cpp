
#include <Tracker.h>
#define DSerial SerialUSB
#define ATSerial Serial1

JsonDocument docOutput;

Tracker tracker(ATSerial, DSerial, docOutput);

void setup(void)
{
    Serial.begin(115200);
    // Serial.println("BMA456 Step Counter");

    tracker.firstStart();
}

void loop(void)
{
    
}
