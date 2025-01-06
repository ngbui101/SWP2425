#include "PowerSavingMode.h"

PowerSavingMode::PowerSavingMode(Tracker &trackerObj)
    : tracker(trackerObj)
{
}


void PowerSavingMode::start() {
    setup();
    loop();
}


void PowerSavingMode::setup() {
    
}

// Hauptschleife (z. B. zyklische Abfragen, Publikationen etc.)
void PowerSavingMode::loop() {
    
}
