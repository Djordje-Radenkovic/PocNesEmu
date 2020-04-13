#pragma once

#include "IMapper.h"


class Mapper_000 : public IMapper {
public:
	Mapper_000(uint8_t prgBanks, uint8_t chrBanks) : IMapper(prgBanks, chrBanks) {}

	MIRROR_MODE getMirrorMode() {
		return MIRROR_MODE::SOLDERED;
	}

private:
	// From IMapper
	bool cpuRead(uint16_t address) override {
		if (address >= 0x8000 && address <= 0xFFFF) {
			m_mappedAddress = address & (m_PRGBanks > 1 ? 0x7FFF : 0x3FFF);
			return true;
		}

		return false;
	}

	bool cpuWrite(uint16_t address, uint8_t data) override { return false; }

	bool ppuRead(uint16_t address) override {
		if (address >= 0x0000 && address <= 0x1FFF) {
			m_mappedAddress = address;
			return true;
		}

		return false;
	}

	bool ppuWrite(uint16_t address) override { return false; }
	// ------------

};
