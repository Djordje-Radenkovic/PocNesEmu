#include "fmt/printf.h"

#include "NesVectorBus.h"


void NesVectorBus::addSlave(
	std::shared_ptr<IBusSlave<uint16_t, uint8_t>> slave,
	uint16_t startAddress, uint16_t endAddress) {

	// TODO: Check for valid address

	// TODO: Check for overlap

	slave->setAddressRange(startAddress, endAddress);
	m_slaves.push_back(slave);
}

void NesVectorBus::addSlave(
	std::shared_ptr<IBusSlave<uint16_t, uint8_t>> slave,
	uint16_t startAddress) {

	// TODO: Check for valid address

	// TODO: Check for overlap

	slave->setAddressRange(startAddress);
	m_slaves.push_back(slave);
}

std::shared_ptr<IBusSlave<uint16_t, uint8_t>>
NesVectorBus::getSlaveWithAddress(uint16_t address) {
	for (int i = 0; i < m_slaves.size(); i++) {
		if (address >= m_slaves[i]->startAddress() && address <= m_slaves[i]->endAddress())
			return m_slaves[i];
	}

	return nullptr;
}

bool NesVectorBus::write(uint16_t address, uint8_t data, bool log) {
	m_tempSlave = getSlaveWithAddress(address);
	if (m_tempSlave == nullptr)
		return false;

	m_tempSlave->write(address, data);
	
	return true;
}

uint8_t NesVectorBus::read(uint16_t address, bool log, bool readOnly) {
	m_tempSlave = getSlaveWithAddress(address);
	if (m_tempSlave == nullptr)
		return -1;
	
	return m_tempSlave->read(address);
}

void NesVectorBus::dump_memory(const char* filePath,
	uint16_t startAddress, uint16_t endAddress) {

	m_memDumpFile.open(filePath, std::ofstream::out);

	if (!m_memDumpFile.is_open()) {
		fmt::print("Failed to open memdump.log file!\n");
		return;
	}

	for (int i = 0; i <= 0xFFFF; i++) {
		if (i % 0x10 == 0) {
			fmt::fprintf(m_memDumpFile, "\n0x%04X: ", i);
		}
		fmt::fprintf(m_memDumpFile, "%02X ", read(i));
	}
	m_memDumpFile.close();
	fmt::printf("Dumped memory to disk ($%04X-$%04X).\n", startAddress, endAddress);
}