#ifndef _MODULE_H_
#define _MODULE_H_

#include <Arduino.h>
#include <Wire.h>
#include "Logger.h"

class _Module {
public:
    _Module();
    ~_Module();
protected:
    Logger initLogger;
    Logger runningLogger;
private:
};

#endif // _MODULE_H_
