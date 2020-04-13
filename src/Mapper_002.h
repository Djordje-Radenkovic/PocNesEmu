#pragma once

#include "IMapper.h"


class Mapper_002 : public IMapper {
public:
	Mapper_002(uint8_t prgBanks, uint8_t chrBanks) 
		: IMapper(prgBanks, chrBanks), selectedBankHi(prgBanks - 1) {}

	MIRROR_MODE getMirrorMode() {
		return MIRROR_MODE::SOLDERED;
	}

private:
	// From IMapper
	bool cpuRead(uint16_t address) override {
		if (address <= 0xBFFF) {
			m_mappedAddress = selectedBankLo * 0x4000 + (address & 0x3FFF);
			return true;
		}
		else {
			m_mappedAddress = selectedBankHi * 0x4000 + (address & 0x3FFF);
			return true;
		}

		return false;
	}

	bool cpuWrite(uint16_t address, uint8_t data) override { 
		selectedBankLo = data & 0x0F;

		return true;
	}

	bool ppuRead(uint16_t address) override {
		if (address >= 0x0000 && address <= 0x1FFF) {
			m_mappedAddress = address;
			return true;
		}

		return false;
	}

	bool ppuWrite(uint16_t address) override { return false; }
	// ------------

private:
	uint8_t selectedBankLo = 0x00;
	uint8_t selectedBankHi = 0x00;

};

