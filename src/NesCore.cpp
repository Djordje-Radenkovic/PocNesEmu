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
	m_cpuBus->addSlave(m_ram, 0x0000, 0x17FF);

	// Add the PPU to the CPU Bus
	m_cpuBus->addSlave(m_ppu, 0x2000, 0x3FFF);

	// Connect PPU to its bus
	m_ppu->connectBus(m_ppuBus);

	// Add PPU's RAMs to its bus
	m_patternTable = std::make_shared<NesArrayRam>(0x2000);
	m_nameTable	   = std::make_shared<NesArrayRam>(0x1000);
	m_palletteRam  = std::make_shared<NesArrayRam>(0x20);

	//// Do this only if cartridge doesn't provide it
	//m_ppuBus->addSlave(m_patternTable, 0x0000);

	m_ppuBus->addSlave(m_nameTable, 0x2000, 0x3EFF);
	m_ppuBus->addSlave(m_palletteRam, 0x3F00, 0x3FFF);
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
}


bool NesCore::loadCartridge(const char* filePath) {
	// Construct Cartridge (which also loads file into it)
	m_cartridge = std::make_shared<NesCartridge>(filePath);

	// Connect Cartridge to Buses
	m_cpuBus->addSlave(m_cartridge, 0x8000, 0xFFFF);
	m_ppuBus->addSlave(m_cartridge, 0x0000, 0x1FFF);

	return true;
}


void NesCore::tick() {
	/*	// MOVE THIS INTO PPUs tick()
	 *
	 *	// Draw pixel
	 *	drawPixel(m_cycle-1, scanline, color);
	 *
	 *	if (m_cycle >= 341) {
	 *		m_cycle = 0;
	 *		scanline++;
	 *		if (scanline >= 261) {
	 *			scanline = -1;
	 *			frameComplete = true;
	 *		}
	 *	}
	 */

	// PPU clocks 3 times faster than the CPU
	m_ppu->tick();
	if (m_totalCyclesPassed % 3 == 0)
		m_cpu->tick();

	m_totalCyclesPassed++;

	// Maybe stop execution?
	/*	if (<exit condition>)
	 *		isOn = false;
	 */
}


void NesCore::powerOff() {
	// Some cleanup here
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


void NesCore::nesTest(const char* romFilePath, const char* memDumpFilePath) {
	loadCartridge(romFilePath);

	// Run the CPU
	runCPU_nCycles(26554, 0xC000);		// NESTest runs for 26554 cycles
										// starting from address $C000

	// Memory dump
	fmt::print("\n");
	m_cpuBus->dump_memory(memDumpFilePath);
}
