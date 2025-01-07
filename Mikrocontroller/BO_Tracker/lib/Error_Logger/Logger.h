#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <Arduino.h>
#include "BG96_Serial.h"

#define MAX_ERRORS 20
#define MAX_ERROR_LENGTH 20

class Logger {
public:
    Logger();
    ~Logger();
    bool logError(const char* message);
    void flushErrors();
    int getErrorCount() const;
    bool getAllError(char *errors);
    void clear();

    
private:
    int errorCount;
    char errors[MAX_ERRORS][MAX_ERROR_LENGTH];
};

#endif // _LOGGER_H_
