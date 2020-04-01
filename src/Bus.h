#pragma once

#include <map>
#include <limits>

#include "fmt/printf.h"

#include "IBusSlave.h"

template <typename addressWidth, typename dataWidth>
class Bus {
private:
	static const int maxAddress = std::numeric_limits<addressWidth>::max();
	addressWidth lastRetrievedStartAddress = 0;

	std::multimap<IBusSlave<addressWidth, dataWidth>*, addressWidth> m_slaves;

public:
	~Bus() {

	}

	void addSlave(IBusSlave<addressWidth, dataWidth>* slaveToAdd, addressWidth startAddressToAdd) {
		// Check if the end address goes above the address space
		if ((startAddressToAdd + slaveToAdd->size()-1) > maxAddress) {
			throw std::overflow_error("Slave's end address overflows possible address space");
			return;
		}

		// Check every existing slave for overlap with new slave
		for (auto it = m_slaves.begin(); it != m_slaves.end(); ++it) {
			// Existing slave's addresses
			IBusSlave<addressWidth, dataWidth>* slave = it->first;
			addressWidth startAddress = it->second;
			addressWidth size = slave->size();
			addressWidth endAddress = startAddress + size-1;

			// New slave's end address
			addressWidth endAddressToAdd = startAddressToAdd + slaveToAdd->size()-1;

			// Check for overlap
			if (endAddressToAdd >= startAddress && endAddressToAdd < endAddress) {
				throw std::range_error("Slave addresses overlaping");
				return;
			}
			else if (startAddressToAdd >= startAddress && startAddressToAdd < endAddress) {
				throw std::range_error("Slave addresses overlaping");
				return;
			}
			else if (startAddress >= startAddressToAdd && endAddress < endAddressToAdd) {
				throw std::range_error("Slave addresses overlaping");
				return;
			}
		}

		// Add new slave
		fmt::printf("Added slave: $%04X-$%04X\n", (int)startAddressToAdd, startAddressToAdd + slaveToAdd->size() - 1);
		m_slaves.insert(std::make_pair(slaveToAdd, startAddressToAdd));
	}

	IBusSlave<addressWidth, dataWidth>* getSlaveWithAddress(addressWidth address) {
		// Check if address is in address space
		if (address <= 0 || address > maxAddress) return nullptr;

		// Find slave that occupies the address and return it
		for (auto it = m_slaves.begin(); it != m_slaves.end(); ++it) {
			IBusSlave<addressWidth, dataWidth>* slave = it->first;
			addressWidth startAddress = it->second;

			if ((address >= startAddress) && (address <= (startAddress + slave->size()-1))) {
				lastRetrievedStartAddress = startAddress;
				return slave;
			}
		}

		// If no slave has the address return nullptr
		return nullptr;
	}

	void write(addressWidth address, dataWidth data, bool log = false) {
		// Write to appropriate slave
		auto slave = getSlaveWithAddress(address);
		if (slave == nullptr) {
			if (log)
				fmt::printf("No slaves match the address %04X. No write will happen.", (int)address);
			return;
		}

		slave->write(address - lastRetrievedStartAddress, data);
	}

	dataWidth read(addressWidth address, bool log = false, bool readOnly = false) {
		// Read from appropriate slave
		auto slave = getSlaveWithAddress(address);
		if (slave == nullptr) {
			if (log)
				fmt::printf("No slaves match the address %04X. Read will return -1 (0xFF).", (int)address);
			return -1;
		}

		return slave->read(address - lastRetrievedStartAddress);
	}

};
