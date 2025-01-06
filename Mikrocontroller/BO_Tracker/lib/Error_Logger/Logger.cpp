#include "Logger.h"
#include <string.h>

Logger::Logger() : errorCount(0) {}

Logger::~Logger() {}

bool Logger::logError(const char* message) {
    if (errorCount < MAX_ERRORS) {
        strncpy(errors[errorCount], message, MAX_ERROR_LENGTH - 1);
        errors[errorCount][MAX_ERROR_LENGTH - 1] = '\0';
        errorCount++;
        return true;
    }
    return false;
}

void Logger::flushErrors(Stream& serial) {
#ifdef UART_DEBUG
    serial.print("Errors: ");
    serial.println(errorCount);
    for (int i = 0; i < errorCount; i++) {
        serial.println(errors[i]);
    }
#endif
}

int Logger::getErrorCount() const {
    return errorCount;
}

bool Logger::getAllError(char *errorsBuffer) {
    if (errorCount == 0) {
        errorsBuffer[0] = '\0';
        return true;
    }

    int currentPos = 0;
    for (int i = 0; i < errorCount; i++) {
        int len = strlen(this->errors[i]);
        // Append the error
        strcpy(&errorsBuffer[currentPos], this->errors[i]);
        currentPos += len;

        // Append comma and space if not the last error
        if (i < errorCount - 1) {
            errorsBuffer[currentPos++] = ',';
            errorsBuffer[currentPos++] = ' ';
            errorsBuffer[currentPos] = '\0'; 
        }
    }

    return true;
}

void Logger::clear() {
    errorCount = 0; 
    for(int i = 0; i < MAX_ERRORS; i++) {
        errors[i][0] = '\0';
    }
}