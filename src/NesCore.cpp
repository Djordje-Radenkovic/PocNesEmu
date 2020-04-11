#include <fstream>
#include <iostream>

#include "fmt/printf.h"

#include "NesCore.h"
#include "NesCartridge.h"


NesCore::NesCore(
	std::shared_ptr<INesCpu> cpu,
	std::shared_ptr<INesPpu> ppu,
	std::shared_ptr<IRam<uint16_t, uint8_t>> ram,
	std::shared_ptr<IBus<uint16_t, uint8_t>> cpuBus,
	std::shared_ptr<IBus<uint16_t, uint8_t>> ppuBus
) : m_cpu(cpu), m_ppu(ppu), m_ram(ram), m_cpuBus(cpuBus), m_ppuBus(ppuBus) {

	m_totalCyclesPassed = 0;

	// Connect CPU to its bus
	m_cpu->connectBus(m_cpuBus);

	// Add the system RAM to the bus
	m_cpuBus->mapSlave(m_ram, 0x0000, 0x17FF);

	// Add the PPU to the CPU Bus
	m_cpuBus->mapSlave(m_ppu, 0x2000, 0x3FFF);

	// Connect PPU to its bus
	m_ppu->connectBus(m_ppuBus);

	// Add PPU's RAMs to its bus
	m_patternTable = std::make_shared<NesArrayRam>(0x2000);
	m_nameTable0   = std::make_shared<NesArrayRam>(0x400);
	m_nameTable1   = std::make_shared<NesArrayRam>(0x400);
	m_palletteRam  = std::make_shared<NesArrayRam>(0x20);

	//// Do this only if cartridge doesn't provide it
	//m_ppuBus->mapSlave(m_patternTable, 0x0000);

	m_ppuBus->mapSlave(m_palletteRam, 0x3F00, 0x3FFF);

}


void NesCore::powerOn() {
	m_isOn = true;

	reset();

	if (m_realTime) {
		while (m_isOn)
			tick();
	}
	else {
		while (m_isOn) {
			tick();
			std::cin.get();
		}
	}

	powerOff();
}


void NesCore::reset() {
	m_totalCyclesPassed = 0;
	m_cpu->reset();
	m_ppu->reset();
}


bool NesCore::loadCartridge(const char* filePath) {
	// Construct Cartridge (which also loads file into it)
	m_cartridge = std::make_shared<NesCartridge>(filePath);

	if (!m_cartridge->isLoaded()) {
		fmt::print("Failed to load cartrigde!\n");
		return false;
	}

	// Connect Cartridge CPU and PPU
	m_cpuBus->mapSlave(m_cartridge, 0x8000, 0xFFFF);
	m_ppuBus->mapSlave(m_cartridge, 0x0000, 0x1FFF);

	// Map Nametables based on the mirroring
	// mode of the cartridge
	switch (m_cartridge->mirrorMode)
	{
	case NesCartridge::MIRROR_MODE::VERTICAL:
		m_ppuBus->mapSlave(m_nameTable0, 0x2000);
		m_ppuBus->mapSlave(m_nameTable0, 0x2800);

		m_ppuBus->mapSlave(m_nameTable1, 0x2400);
		m_ppuBus->mapSlave(m_nameTable1, 0x2C00);
		break;
	case NesCartridge::MIRROR_MODE::HORIZONTAL:
		m_ppuBus->mapSlave(m_nameTable0, 0x2000, 0x27FF);
		m_ppuBus->mapSlave(m_nameTable1, 0x8000, 0x2FFF);
		break;
	case NesCartridge::MIRROR_MODE::ONE_SCREEN:
		m_ppuBus->mapSlave(m_nameTable0, 0x2000, 0x2FFF);
		break;
	case NesCartridge::MIRROR_MODE::FOUR_SCREEN:
		// TODO: Implement this
		fmt::print("Four screen mirror mode not implemented!");
		return false;
	default:
		fmt::print("Unsupported nametable mirroring mode!");
		return false;
	}


	return true;
}


void NesCore::tick() {
	// PPU clocks 3 times faster than the CPU
	m_ppu->tick();
	if (m_totalCyclesPassed % 3 == 0)
		m_cpu->tick();

	// Interrupt CPU if needed
	if (m_ppu->getNmi()) {
		m_ppu->clearNmi();
		m_cpu->nmi();
	}

	m_totalCyclesPassed++;

	// If some component isn't running
	// stop the NES
	if (!m_ppu->isRunning())
		powerOff();
}


void NesCore::powerOff() {
	// Some cleanup here
	m_isOn = false;

	// Memory dump
	fmt::print("\n");
	m_cpuBus->dump_memory("./logs/cpudump.log");
	m_ppuBus->dump_memory("./logs/ppudump.log");
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

void NesCore::runCPU_nCycles(size_t nCycles, uint16_t pc) {
	m_cpu->reset(pc);
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

void NesCore::runCPU_nInstructions(size_t nInstructions, uint16_t pc) {
	m_cpu->reset(pc);
	do {
		m_cpu->tick();
		if (m_cpu->isFinished())
			nInstructions--;
	} while (nInstructions > 0);
}


void NesCore::nesTest(const char* romFilePath, const char* memDumpFilePath,
	bool noPpu) {

	if (!loadCartridge(romFilePath))
		return;

	// Run the CPU
	if (noPpu)
		runCPU_nCycles(26554, 0xC000);		// NESTest runs for 26554 cycles
	else									// starting from address $C000
		powerOn();

	// Memory dump
	fmt::print("\n");
	m_cpuBus->dump_memory(memDumpFilePath);
	m_ppuBus->dump_memory("./logs/ppudump.log");
}
