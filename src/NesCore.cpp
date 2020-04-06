#include <fstream>

#include "fmt/printf.h"

#include "NesCore.h"
#include "Config.h"


NesCore::NesCore(
	std::shared_ptr<INesCpu> cpu,
	std::shared_ptr<INesPpu> ppu,
	std::shared_ptr<IRam<uint16_t, uint8_t>> ram,
	std::shared_ptr<IBus<uint16_t, uint8_t>> cpuBus,
	std::shared_ptr<IBus<uint16_t, uint8_t>> ppuBus
) : m_cpu(cpu), m_ppu(ppu), m_ram(ram), m_cpuBus(cpuBus), m_ppuBus(ppuBus) {

	totalCyclesPassed = 0;

	// Connect CPU to its bus
	m_cpu->connectBus(m_cpuBus);

	// Add the system RAM to the bus
	m_cpuBus->addSlave(m_ram, 0x0000, 0x1800);

	// Add the PPU to the CPU Bus
	m_cpuBus->addSlave(m_ppu, 0x2000, 0x3FFF);

	// Connect PPU to its bus
	m_ppu->connectBus(m_ppuBus);

	// Add PPU's RAMs to its bus
	m_patternTable = std::make_shared<NesArrayRam>(0x2000);
	m_nameTable	   = std::make_shared<NesArrayRam>(0x1000);
	m_palletteRam  = std::make_shared<NesArrayRam>(0x20);

	m_ppuBus->addSlave(m_patternTable, 0x0000);
	m_ppuBus->addSlave(m_nameTable, 0x2000, 0x3EFF);
	m_ppuBus->addSlave(m_palletteRam, 0x3F00, 0x3FFF);
}


void NesCore::reset() {
	totalCyclesPassed = 0;
	m_cpu->reset();
}


bool NesCore::loadCartridge(const char* filePath) {
	return true;
}



//	+---------------------------+
//	|	Testing/Debug Methods	|
//	+---------------------------+

void NesCore::runCPU_nCycles(size_t nCycles) {
	m_cpu->reset();
	do {
		m_cpu->tick();
	} while (m_cpu->getCyclesPassed() <= nCycles);
}


void NesCore::runCPU_nInstructions(size_t nInstructions) {
	m_cpu->reset();
	do {
		m_cpu->tick();
		if (m_cpu->isFinished())
			nInstructions--;
	} while (nInstructions > 0);
}


void NesCore::nesTest() {
	// Load ROM
	std::streampos size;
	uint8_t* memblock = nullptr;

	std::ifstream nesTestRom(NESTEST_FILE_PATH,
		std::ios::binary | std::ios::in | std::ios::ate);

	if (!nesTestRom.is_open()) {
		fmt::print("Couldn't open ROM file!\n");

		return;
	}

	size = nesTestRom.tellg();
	memblock = new uint8_t[size];
	nesTestRom.seekg(0, std::ios::beg);
	nesTestRom.read((char*)memblock, size);
	const int romSize = nesTestRom.gcount();
	nesTestRom.close();

	fmt::print("ROM file read.\n");

	const uint16_t baseAddress = 0x8000;
	const uint16_t otherAddress = 0xC000;
	const uint16_t romOffset = 0x0010;

	m_cartridge = std::make_shared<NesArrayRam>(0x4000);
	m_cpuBus->addSlave(m_cartridge, 0x8000, 0xFFFF);

	for (int i = 0; i < 0x4000; i++) {
		m_cartridge->write(i, memblock[romOffset + i]);
	}

	delete[] memblock;

	fmt::print("ROM loaded into memory.\n");

	// Write reset vector
	m_cpuBus->write(0xFFFC, 0x00);
	m_cpuBus->write(0xFFFD, 0xC0);

	// Run the CPU
	runCPU_nCycles(26554);		// NESTest runs for 26554 cycles

	// Memory dump
	fmt::print("\n");
	m_cpuBus->dump_memory(MEM_DUMP_FILE_PATH);
}
