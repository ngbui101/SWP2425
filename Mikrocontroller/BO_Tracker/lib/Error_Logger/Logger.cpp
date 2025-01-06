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
