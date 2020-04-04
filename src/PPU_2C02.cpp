#include "PPU_2C02.h"


void PPU_2C02::connectBus(Bus<uint16_t, uint8_t>* bus) {
	m_bus = bus;
}

uint16_t PPU_2C02::size() {
	return 0x8;
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
