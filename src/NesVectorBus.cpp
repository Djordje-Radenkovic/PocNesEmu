#include "NesVectorBus.h"


NesVectorBus::~NesVectorBus() { 
	m_slaves.clear();
	m_starts.clear();
	m_ends.clear();
}

void NesVectorBus::addSlave(IBusSlave<uint16_t, uint8_t>* slaveToAdd,
	uint16_t startAddressToAdd, uint16_t endAddressToAdd) {

	// Determine end address
	if (!endAddressToAdd) {
		endAddressToAdd = startAddressToAdd + slaveToAdd->size() - 1;
	}

	// Check if addresses are in address space range
	if (startAddressToAdd < 0 || startAddressToAdd > maxAddress
		|| endAddressToAdd > maxAddress || endAddressToAdd <= 0 ||
		endAddressToAdd <= startAddressToAdd) {

		throw std::range_error("Invalid slave start and/or end address.");
		return;
	}

	// Check for overlap with existing slaves
	for (int i = 0; i < m_slaves.size(); i++) {
		for (int j = 0; j < m_starts[i].size(); j++) {
			if (endAddressToAdd >= m_starts[i][j] && endAddressToAdd <= m_ends[i][j]) {
				throw std::range_error("Slave addresses overlaping");
				return;
			}
			else if (startAddressToAdd >= m_starts[i][j] && startAddressToAdd <= m_ends[i][j]) {
				throw std::range_error("Slave addresses overlaping");
				return;
			}
			else if (m_starts[i][j] >= startAddressToAdd && m_ends[i][j] <= endAddressToAdd) {
				throw std::range_error("Slave addresses overlaping");
				return;
			}
		}
	}

	// Check if slave already exists
	for (int i = 0; i < m_slaves.size(); i++) {
		if (m_slaves[i] == slaveToAdd) {
			existingIndex = i;
			break;
		}
	}

	// Add slave
	if (existingIndex != -1) {
		m_starts[existingIndex].push_back(startAddressToAdd);
		m_ends[existingIndex].push_back(endAddressToAdd);
	}
	else {
		m_slaves.push_back(slaveToAdd);
		m_starts.push_back({ startAddressToAdd });
		m_ends.push_back({ endAddressToAdd });
	}
}


IBusSlave<uint16_t, uint8_t>* NesVectorBus::getSlaveWithAddress(uint16_t address) {
	for (int i = 0; i < m_slaves.size(); i++) {
		for (int j = 0; j < m_starts[i].size(); j++) {
			if (address >= m_starts[i][j] && address <= m_ends[i][j]) {
				lastRetrievedStartAddress = m_starts[i][j];
				return m_slaves[i];
			}
		}
	}

	return nullptr;
}


bool NesVectorBus::write(uint16_t address, uint8_t data, bool log) {
	m_tempSlave = getSlaveWithAddress(address);
	if (m_tempSlave == nullptr) {
		return false;
	}

	m_tempSlave->write(address % m_tempSlave->size(), data);

	return true;
}


uint8_t NesVectorBus::read(uint16_t address, bool log, bool readonly) {
	m_tempSlave = getSlaveWithAddress(address);
	if (m_tempSlave == nullptr) {
		return -1;
	}

	return m_tempSlave->read(address % m_tempSlave->size());
}
