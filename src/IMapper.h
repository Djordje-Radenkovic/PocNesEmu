#pragma once

#include <cstdint>
#include "fmt/printf.h"


class IMapper {
public:
	IMapper(uint8_t prgBanks, uint8_t chrBanks) : 
		m_PRGBanks(prgBanks), m_CHRBanks(chrBanks) {}

	uint16_t mapRead(uint16_t address) {
		if (address >= 0x0000 && address <= 0x1FFF)
			m_result = ppuRead(address);
		if (address >= 0x8000 && address <= 0xFFFF)
			m_result = cpuRead(address);

		if (m_result)
			return m_mappedAddress;

		fmt::print("MMC failed to map read address!");
		return -1;
	}

	uint16_t mapWrite(uint16_t address) {
		if (address >= 0x0000 && address <= 0x1FFF)
			return ppuWrite(address);
		if (address >= 0x8000 && address <= 0xFFFF)
			return cpuWrite(address);

		if (m_result)
			return m_mappedAddress;

		fmt::print("MMC failed to map write address!");
		return -1;
	}

protected:
	virtual bool cpuRead(uint16_t address) = 0;
	virtual bool cpuWrite(uint16_t address) = 0;

	virtual bool ppuRead(uint16_t address) = 0;
	virtual bool ppuWrite(uint16_t address) = 0;

protected:
	bool m_result = false;
	uint16_t m_mappedAddress = 0;
	uint8_t m_PRGBanks = 0;
	uint8_t m_CHRBanks = 0;

};
