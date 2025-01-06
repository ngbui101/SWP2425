#include <EventHandler.h>
#include <RealTimeMode.h>

#define DSerial SerialUSB
#define ATSerial Serial1

// De- und Serialisation
JsonDocument docInput;
JsonDocument docOutput;

Tracker tracker(ATSerial, DSerial,docInput);

RealTimeMode realtimeTracker(tracker);
bool realtime = false;
void setup()
{ 
  DSerial.begin(115200);
  while (DSerial.read() >= 0)
    ; // Buffer leeren
  delay(3000);
  ATSerial.begin(115200);
  while (ATSerial.read() >= 0)
    ; // Buffer leeren
  delay(3000);
  tracker.InitModule();
}


void loop()
{
  if(realtime){
    realtimeTracker.start();
  }else{
    ///longtimeTracker.start();
  }
}
