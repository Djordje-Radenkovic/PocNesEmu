#pragma once

#include <cstdint>

#include "IBusMaster.h"


class INesCpu : public IBusMaster<uint16_t, uint8_t> {
protected:
	virtual void irq() = 0;
	virtual void nmi() = 0;

	virtual inline uint8_t readFrom(uint16_t address) = 0;
	inline void writeTo(uint16_t address, uint8_t data) = 0;
public:
	virtual void reset() = 0;
	virtual void tick() = 0;
	virtual void connectBus(IBus<uint16_t, uint8_t>* bus) = 0;
	virtual bool isFinished() = 0;
	virtual size_t getCyclesPassed() = 0;

	// Makes the interface not instantiable
	virtual ~INesCpu() {};
};
