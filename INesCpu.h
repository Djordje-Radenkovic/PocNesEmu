#pragma once

#include <string>

#include "Bus.h"

class INesCpu {
protected:
	virtual void irq() = 0;
	virtual void nmi() = 0;
public:
	virtual void reset() = 0;
	virtual void tick() = 0;
	virtual void connectBus(Bus<uint16_t, uint8_t>* bus) = 0;
	virtual bool isFinished() = 0;
	virtual size_t getCyclesPassed() = 0;

	// Makes the interface not instantiable
	virtual ~INesCpu() = 0 {};
};
