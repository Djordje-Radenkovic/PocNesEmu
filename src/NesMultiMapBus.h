#pragma once

#include <map>

#include "fmt/printf.h"

#include "IBus.h"


class NesMultiMapBus : public IBus<uint16_t, uint8_t> {
public:
	void addSlave(std::shared_ptr<IBusSlave<uint16_t, uint8_t>> slaveToAdd,
		uint16_t startAddressToAdd, uint16_t endAddressToAdd = 0) override;

	std::shared_ptr<IBusSlave<uint16_t, uint8_t>> 
		getSlaveWithAddress(uint16_t address) override;

	bool write(uint16_t address, uint8_t data, bool log = false) override;
	uint8_t read(uint16_t address,
		bool log = false, bool readOnly = false) override;

private:
	uint16_t lastRetrievedStartAddress = 0;
	std::shared_ptr<IBusSlave<uint16_t, uint8_t>> m_tempSlave = nullptr;

	std::multimap<
		std::shared_ptr<IBusSlave<uint16_t, uint8_t>>, uint16_t
	> m_slaves;

};
