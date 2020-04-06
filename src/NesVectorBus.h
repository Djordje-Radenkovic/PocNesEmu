#pragma once

#include <vector>

#include "fmt/printf.h"

#include "IBus.h"


class NesVectorBus : public IBus<uint16_t, uint8_t> {
private:
	size_t existingIndex = -1;
	std::shared_ptr<IBusSlave<uint16_t, uint8_t>> m_tempSlave = nullptr;
	uint16_t lastRetrievedStartAddress = 0;

	std::vector<std::shared_ptr<IBusSlave<uint16_t, uint8_t>>> m_slaves;
	std::vector<std::vector<uint16_t>> m_starts;
	std::vector<std::vector<uint16_t>> m_ends;

public:
	void addSlave(std::shared_ptr<IBusSlave<uint16_t, uint8_t>> slaveToAdd,
		uint16_t startAddressToAdd, uint16_t endAddressToAdd = 0) override;

	std::shared_ptr<IBusSlave<uint16_t, uint8_t>>
		getSlaveWithAddress(uint16_t address) override;

	bool write(uint16_t address, uint8_t data, bool log = false) override;
	uint8_t read(uint16_t address,
		bool log = false, bool readOnly = false) override;

};
