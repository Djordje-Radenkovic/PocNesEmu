#include "fmt/printf.h"

#include "NesMultiMapBus.h"


void NesMultiMapBus::addSlave(
	std::shared_ptr<IBusSlave<uint16_t, uint8_t>> slave,
	uint16_t startAddress, uint16_t endAddress) {

	m_addSlave(slave, startAddress, endAddress);
}


void NesMultiMapBus::addSlave(
	std::shared_ptr<IBusSlave<uint16_t, uint8_t>> slave,
	uint16_t startAddress) {
	
	uint16_t endAddress = startAddress + slave->size() - 1;
	m_addSlave(slave, startAddress, endAddress);
}


void NesMultiMapBus::m_addSlave(
	std::shared_ptr<IBusSlave<uint16_t, uint8_t>> slaveToAdd,
	uint16_t startAddress, uint16_t endAddress) {

	// Check if the end address goes above the address space
	if (endAddress > maxAddress) {
		throw std::overflow_error("Slave's end address overflows possible address space");
		return;
	}

	// Check every existing slave for overlap with new slave
	for (auto it = m_slaves.begin(); it != m_slaves.end(); ++it) {
		// Existing slave's addresses
		m_tempSlave = it->first;
		std::array<uint16_t, 2>& edges = it->second;

		// Check for overlap
		if (endAddress >= edges[0] && endAddress < edges[1]) {

			throw std::range_error("Slave addresses overlaping");
			return;
		}
		else if (startAddress >= edges[0] && startAddress < edges[1]) {

			throw std::range_error("Slave addresses overlaping");
			return;
		}
		else if (edges[0] >= startAddress && edges[1] < endAddress) {

			throw std::range_error("Slave addresses overlaping");
			return;
		}
	}

	// Add new slave
	fmt::printf("Added slave: $%04X-$%04X\n", 
		(int)startAddress, endAddress);

	m_slaves.insert(std::make_pair(
		slaveToAdd, std::array<uint16_t, 2> {startAddress, endAddress}
	));
}


void NesMultiMapBus::getSlaveWithAddress(uint16_t address) {
	// Check if address is in address space
	if (address < 0 || address > maxAddress) return;

	// Find slave that occupies the address and return it
	for (auto it = m_slaves.begin(); it != m_slaves.end(); ++it) {
		m_tempSlave = it->first;
		std::array<uint16_t, 2>& edges = it->second;

		if (address >= edges[0] && address <= edges[1]) {
			lastRetrievedStartAddress = edges[0];
			return;
		}
	}

	// If no slave has the address return nullptr
	m_tempSlave = nullptr;
	return;
}


bool NesMultiMapBus::write(uint16_t address, uint8_t data) {
	// Write to appropriate slave
	getSlaveWithAddress(address);
	if (m_tempSlave == nullptr)
		return false;

	m_tempSlave->write(address % m_tempSlave->size(), data);

	return true;
}


uint8_t NesMultiMapBus::read(uint16_t address) {
	// Read from appropriate slave
	getSlaveWithAddress(address);
	if (m_tempSlave == nullptr)
		return -1;

	return m_tempSlave->read(address % m_tempSlave->size());
}


void NesMultiMapBus::dump_memory(const char* filePath,
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
