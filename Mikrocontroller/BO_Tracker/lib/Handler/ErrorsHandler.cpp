#include "ErrorsHandler.h"

ErrorsHandler::ErrorsHandler(Stream &atSerial, Stream &dSerial, JsonDocument &doc)
    : m_atSerial(atSerial), m_dSerial(dSerial), m_doc(doc)
{
    
}



// Methode zum Ersetzen des aktuellen Trackers
bool ErrorsHandler::renewTracker(Tracker &tracker)
{   
    tracker.turnOffModem();

    return true;
}
