#include "RealTimeMode.h"

RealTimeMode::RealTimeMode(Tracker &trackerObj)
    : tracker(trackerObj)
{
}


void RealTimeMode::start() {
    setup();
    loop();
}


void RealTimeMode::setup() {
    
}

// Hauptschleife (z. B. zyklische Abfragen, Publikationen etc.)
void RealTimeMode::loop() {
    
}
