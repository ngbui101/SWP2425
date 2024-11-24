
#ifndef __BOARD_H_
#define __BOARD_H_

#include "Temperature.h"

class _Board : public _Temperature{
    public:
    _Board();
    boolean initBoard();
};
#endif
