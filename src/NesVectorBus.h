#pragma once

#include <vector>
#include <fstream>

#include "IBus.h"
#include "IBusSlave.h"


class NesVectorBus : public IBus<uint16_t, uint8_t> {
public:
	void addSlave(std::shared_ptr<IBusSlave<uint16_t, uint8_t>> slave,
		uint16_t startAddress, uint16_t endAddress) override;

	void addSlave(std::shared_ptr<IBusSlave<uint16_t, uint8_t>> slave,
		uint16_t startAddress) override;

	std::shared_ptr<IBusSlave<uint16_t, uint8_t>>
		getSlaveWithAddress(uint16_t address) override;

	bool write(uint16_t address, uint8_t data, bool log = false) override;
	uint8_t read(uint16_t address,
		bool log = false, bool readOnly = false) override;

	void dump_memory(const char* filePath,
		uint16_t startAddress = 0, uint16_t endAddresss = maxAddress) override;

private:
	std::vector<std::shared_ptr<IBusSlave<uint16_t, uint8_t>>> m_slaves;
	std::shared_ptr<IBusSlave<uint16_t, uint8_t>> m_tempSlave;

	std::ofstream m_memDumpFile;

};
