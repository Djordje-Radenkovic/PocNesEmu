#include <fstream>

#include "fmt/printf.h"

#include "NesCartridge.h"


NesCartridge::NesCartridge(const char* romFilePath) {
	std::ifstream romFile;
	romFile.open(romFilePath, std::ifstream::binary);

	if (!romFile.is_open()) {
		fmt::print("Failed to open ROM file!");
		return;
	}

	// Read iNES file header
	romFile.read((char*)&m_header, sizeof(romHeader));

	// Read training information, ignored for now
	// TODO: Read it
	if (m_header.mapper1 & 0x04)
		romFile.seekg(512, std::ios_base::cur);

	// Set mapper information
	m_mapperID = ((m_header.mapper2 >> 4) << 4 | (m_header.mapper1 >> 4));

	// Get type of iNES file
	// TODO: Do this instead of hard coding 1
	m_fileType = 1;

	switch (m_fileType)
	{
	case 0:
		fmt::print("iNES file type 0 not implemented!");
		break;
	case 1:
		// Read PRG memory
		m_PRGBanks = m_header.prg_rom_chunks;
		m_PRGMemorySize = (uint16_t)m_PRGBanks * 16384;
		m_PRGMemory = new uint8_t[m_PRGMemorySize];
		romFile.read((char*)m_PRGMemory, m_PRGMemorySize);

		// Read CHR memory
		m_CHRBanks = m_header.chr_rom_chunks;
		m_CHRMemorySize = (uint16_t)m_CHRBanks * 8192;
		m_CHRMemory = new uint8_t[m_CHRMemorySize];
		romFile.read((char*)m_CHRMemory, m_CHRMemorySize);
		break;
	case 2:
		fmt::print("iNES file type 2 not implemented!");
		break;
	default:
		fmt::print("Unsuported iNES file type!");
		break;
	}

	romFile.close();
}


NesCartridge::~NesCartridge() {
	if (m_PRGMemory != nullptr)
		delete[] m_PRGMemory;

	if (m_CHRMemory != nullptr)
		delete[] m_CHRMemory;
}


// TEMP
inline const uint16_t NesCartridge::size() {
	return 0;
}

uint8_t NesCartridge::read(uint16_t address) {
	return 0;
}

void NesCartridge::write(uint16_t address, uint8_t data) {

}
