#ifndef __BG96_MODULE_H_

#define __BG96_MODULE_H_

#include "BG96_GNSS.h"

class _BG96_Module : public _BG96_GNSS
{
    public:
    _BG96_Module();

    ~_BG96_Module();

    _BG96_Module(Stream &atserial, Stream &dserial);
};

#endif