#include "PPU_2C02.h"


void PPU_2C02::connectBus(std::shared_ptr<IBus<uint16_t, uint8_t>> bus) {
	m_bus = bus;
}

uint8_t PPU_2C02::read(uint16_t address) {
	switch(address) {
	case 0x0000:	// Control
		break;
	case 0x0001:	// Mask
		break;
	case 0x0002:	// Status
		break;
	case 0x0003:	// OAM Address
		break;
	case 0x0004:	// OAM Data
		break;
	case 0x0005:	// Scroll
		break;
	case 0x0006:	// PPU Address
		break;
	case 0x0007:	// PPU Data
		break;
	}

	return -1;
}

void PPU_2C02::write(uint16_t address, uint8_t data) {
	switch (address) {
	case 0x0000:	// Control
		break;
	case 0x0001:	// Mask
		break;
	case 0x0002:	// Status
		break;
	case 0x0003:	// OAM Address
		break;
	case 0x0004:	// OAM Data
		break;
	case 0x0005:	// Scroll
		break;
	case 0x0006:	// PPU Address
		break;
	case 0x0007:	// PPU Data
		break;
	}
}
