
#ifndef __BOARD_H_
#define __BOARD_H_

#include "LowPower.h"

const int wakeUpPin = 0;

class _Board : public _Lowpower {

public:
    _Board(Stream &DSerial);

    ~_Board();
    bool initBoard();

protected:  
    Stream &DSerial;
   
};

#endif
