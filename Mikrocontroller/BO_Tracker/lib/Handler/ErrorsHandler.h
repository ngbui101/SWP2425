#ifndef ERRORSHANDLER_H_
#define ERRORSHANDLER_H_

#include "Tracker.h"

class ErrorsHandler
{
public:
    // Konstruktor
    ErrorsHandler(Stream &atSerial, Stream &dSerial, JsonDocument &doc);
    
    bool renewTracker(Tracker &newTracker); 

private:
    Stream &m_atSerial;     
    Stream &m_dSerial;      
    JsonDocument &m_doc;   
};

#endif
