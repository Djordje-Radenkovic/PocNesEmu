#pragma once

#include <cstdint>

#include "INesPpu.h"


class PPU_2C02 final : public INesPpu {
private:
	inline uint8_t readFrom(uint16_t address) {
		return m_bus->read(address);
	}

	inline void writeTo(uint16_t address, uint8_t data) {
		m_bus->write(address, data);
	}

public:
	void connectBus(IBus<uint16_t, uint8_t>* bus) override;

	uint16_t size() override;
	uint8_t read(uint16_t address) override;
	void write(uint16_t address, uint8_t data) override;
};
