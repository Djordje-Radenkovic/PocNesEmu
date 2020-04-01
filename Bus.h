#pragma once

#include <map>
#include <limits>

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
		printf_s("Added slave: $%04X-$%04X\n", (int)startAddressToAdd, startAddressToAdd + slaveToAdd->size() - 1);
		m_slaves.insert(std::make_pair(slaveToAdd, startAddressToAdd));
	}

	IBusSlave<addressWidth, dataWidth>* getSlaveWithAddress(addressWidth address) {
		// std::cout << "Checking slaves for address " << (int)address << std::endl;

		// Check if address is in address space
		if (address <= 0 || address > maxAddress) return nullptr;

		// Find slave that occupies the address and return it
		for (auto it = m_slaves.begin(); it != m_slaves.end(); ++it) {
			IBusSlave<addressWidth, dataWidth>* slave = it->first;
			addressWidth startAddress = it->second;
			// std::cout << "Checking slave: " << (int)startAddress << "-" << (int)startAddress + slave->size()-1 << std::endl;

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
				std::cout << "No slaves match the address " << (int)address << ". No write will happen." << std::endl;
			return;
		}

		slave->write(address - lastRetrievedStartAddress, data);
	}

	dataWidth read(addressWidth address, bool log = false, bool readOnly = false) {
		// Read from appropriate slave
		auto slave = getSlaveWithAddress(address);
		if (slave == nullptr) {
			if (log)
				std::cout << "No slaves match the address " << (int)address << ". Read will return -1." << std::endl;
			return -1;
		}

		// std::cout << "Reading from slave " << slave->size() << std::endl;

		return slave->read(address - lastRetrievedStartAddress);
	}

};
