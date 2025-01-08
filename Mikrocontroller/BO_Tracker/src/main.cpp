#include <RealTimeMode.h>
#include <PowerSavingMode.h>
#define DSerial SerialUSB
#define ATSerial Serial1

// De- und Serialisation
// JsonDocument docInput;
JsonDocument docOutput;

Tracker tracker(ATSerial, DSerial,docOutput);


RealTimeMode realtimeTracker(tracker);
PowerSavingMode longtimeTracker(tracker);

// int totalReset = 0;

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
  tracker.firstStart();
}

void loop()
{ 

  if(trackerModes.realtime){
    realtimeTracker.start();
  }else{
    longtimeTracker.start();
  }

  if(tracker.checkForError() > 0){
    // totalReset ++;
    tracker.resetModem();
  }
  // if(totalReset )
  delay(100);
}
