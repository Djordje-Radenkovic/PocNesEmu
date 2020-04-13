#pragma once

#include "IBusMaster.h"
#include "IBusSlave.h"


class INesPpu : public IBusMaster<uint16_t, uint8_t>,
    public IBusSlave<uint16_t, uint8_t> {

public:
    virtual void reset() = 0;
    virtual void tick()  = 0;

    virtual bool isRunning() = 0;
    virtual bool getNmi()    = 0;
    virtual void clearNmi()  = 0;

    virtual int inline getCycle()    = 0;
    virtual int inline getScanline() = 0;
};
