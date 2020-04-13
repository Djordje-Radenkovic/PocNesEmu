#pragma once

#include <cstdint>

#include "IBusSlave.h"
#include "NesRom.h"

#include "IMapper.h"


class NesCartridge : public IBusSlave<uint16_t, uint8_t> {
public:
	NesCartridge(const char* romFilePath);
	~NesCartridge();

	bool inline isLoaded() const { return m_isLoaded; }
	MIRROR_MODE getMirorMode();

	// From IBusSlave
	inline const uint16_t size() override;
	uint8_t read(uint16_t address, bool readOnly) override;
	void write(uint16_t address, uint8_t data) override;
	// --------------

private:
	bool m_isLoaded = false;

	std::shared_ptr<IMapper> m_mapper;
	MIRROR_MODE m_mirrorMode;
	romHeader m_header;
	uint8_t m_fileType = 0;

	uint8_t* m_PRGMemory = nullptr;
	uint16_t m_PRGMemorySize = 0;

	uint8_t* m_CHRMemory = nullptr;
	uint16_t m_CHRMemorySize = 0;

	uint8_t m_mapperID = 0;
	uint8_t m_PRGBanks = 0;
	uint8_t m_CHRBanks = 0;

};
