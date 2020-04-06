#include "PPU_2C02.h"


void PPU_2C02::connectBus(std::shared_ptr<IBus<uint16_t, uint8_t>> bus) {
	m_bus = bus;
}

void PPU_2C02::setAddressRange(uint16_t startAddress,
	uint16_t endAddress) {
	m_startAddress = startAddress;
	m_endAddress = endAddress;
}

void PPU_2C02::setAddressRange(uint16_t startAddress) {
	m_startAddress = startAddress;
	m_endAddress = startAddress + m_size - 1;
}

uint8_t PPU_2C02::read(uint16_t address) {
	switch(address) {
	// Check all possible addresses
	}

	return -1;
}

void PPU_2C02::write(uint16_t address, uint8_t data) {
	// Do we even do this?

	switch (address) {
		// Check all possible addresses
	}
}
