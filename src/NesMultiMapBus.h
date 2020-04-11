#pragma once

#include <map>
#include <array>

#include "IBus.h"


class NesMultiMapBus : public IBus<uint16_t, uint8_t> {
public:
	void mapSlave(std::shared_ptr<IBusSlave<uint16_t, uint8_t>> slave,
		uint16_t startAddress, uint16_t endAddress) override;

	void mapSlave(std::shared_ptr<IBusSlave<uint16_t, uint8_t>> slave,
		uint16_t startAddress) override;

	void getSlaveWithAddress(uint16_t address) override;

	bool write(uint16_t address, uint8_t data) override;
	uint8_t read(uint16_t address, bool readOnly = false) override;

	void dump_memory(const char* filePath,
		uint16_t startAddress = 0, uint16_t endAddress = maxAddress) override;

private:
	void m_addSlave(std::shared_ptr<IBusSlave<uint16_t, uint8_t>> slave,
		uint16_t startAddress, uint16_t endAddress);

private:
	uint16_t lastRetrievedStartAddress = 0;
	std::shared_ptr<IBusSlave<uint16_t, uint8_t>> m_tempSlave = nullptr;

	std::multimap<std::shared_ptr<IBusSlave<uint16_t, uint8_t>>,
				  std::array<uint16_t, 2>> m_slaves;

	std::ofstream m_memDumpFile;
};
