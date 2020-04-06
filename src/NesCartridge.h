#pragma once

#include <cstdint>

#include "IBusSlave.h"
#include "NesRom.h"


class NesCartridge : public IBusSlave<uint16_t, uint8_t> {
public:
	NesCartridge(const char* romFilePath);
	~NesCartridge();


	// From IBusSlave
	inline const uint16_t size() override;
	uint8_t read(uint16_t address) override;
	void write(uint16_t address, uint8_t data) override;
	// --------------

private:
	romHeader m_header;
	uint8_t m_fileType;

	uint8_t* m_PRGMemory = nullptr;
	uint16_t m_PRGMemorySize = 0;

	uint8_t* m_CHRMemory = nullptr;
	uint16_t m_CHRMemorySize = 0;

	uint8_t m_mapperID = 0;
	uint8_t m_PRGBanks = 0;
	uint8_t m_CHRBanks = 0;

};
