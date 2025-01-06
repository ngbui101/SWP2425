#include "BG96_Module.h"

_BG96_Module::_BG96_Module()
{
}

_BG96_Module::~_BG96_Module()
{
}

_BG96_Module::_BG96_Module(Stream &atserial, Stream &dserial) : _BG96_GNSS(atserial, dserial) 
{
}