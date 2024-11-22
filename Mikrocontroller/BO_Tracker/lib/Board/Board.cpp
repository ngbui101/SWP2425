#include "Board.h"

_Board::_Board()
{
}
boolean _Board::initBoard()
{
    if (initBattery() && initTemp())
        return true;
    else
        return false;
}
