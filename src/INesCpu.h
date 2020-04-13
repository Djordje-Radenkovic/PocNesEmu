#pragma once

#include <cstdint>
#include <string>

#include "IBusMaster.h"


class INesCpu : public IBusMaster<uint16_t, uint8_t> {
public:
	virtual void reset() = 0;
	virtual void reset(uint16_t pc) = 0;
	virtual void tick() = 0;
	virtual bool isFinished() = 0;
	virtual const inline size_t getCyclesPassed() = 0;
	virtual void nmi() = 0;
	virtual void irq() = 0;
	virtual std::string getLog() = 0;

	virtual ~INesCpu() {}

};
