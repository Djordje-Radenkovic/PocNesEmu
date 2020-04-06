#pragma once

#include "INesCartridge.h"
#include "INesCpu.h"
#include "INesPpu.h"
#include "IBus.h"
#include "IRam.h"
#include "NesArrayRam.h"


class NesCore final {
public:
	NesCore(
		std::shared_ptr<INesCpu> cpu,
		std::shared_ptr<INesPpu> ppu,
		std::shared_ptr<IRam<uint16_t, uint8_t>> ram,
		std::shared_ptr<IBus<uint16_t, uint8_t>> cpuBus,
		std::shared_ptr<IBus<uint16_t, uint8_t>> ppuBus
	);


	void nesTest();

	void powerOn();
	void powerOff();
	void pause();
	void resume();
	void saveState();
	void loadState();
	bool loadCartridge(const char* filePath);

private:
	std::shared_ptr<INesCpu> m_cpu;
	std::shared_ptr<INesPpu> m_ppu;

	std::shared_ptr<IBus<uint16_t, uint8_t>> m_cpuBus;
	std::shared_ptr<IBus<uint16_t, uint8_t>> m_ppuBus;

	std::shared_ptr<IRam<uint16_t, uint8_t>> m_ram;

	std::shared_ptr<IRam<uint16_t, uint8_t>> m_mainRom;
	std::shared_ptr<IRam<uint16_t, uint8_t>> m_otherRom;

	std::shared_ptr<IRam<uint16_t, uint8_t>> m_patternTable;
	std::shared_ptr<IRam<uint16_t, uint8_t>> m_nameTable;
	std::shared_ptr<IRam<uint16_t, uint8_t>> m_palletteRam;

private:
	void runCPU_nCycles(size_t nCycles);
	void runCPU_nInstructions(size_t nInstructions);
	static void dump_memory(std::shared_ptr<IBus<uint16_t, uint8_t>> bus,
		size_t startAddress = 0, size_t endAddress = 0xFFFF);
};
