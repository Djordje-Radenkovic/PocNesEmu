#pragma once

#include "NesCartridge.h"
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


	void nesTest(const char* romFilePath, const char* memDumpFilePath);

	void powerOn();
	void powerOff();
	bool loadCartridge(const char* filePath);
	void reset();
	void tick();

private:
	std::shared_ptr<INesCpu> m_cpu;
	std::shared_ptr<INesPpu> m_ppu;

	std::shared_ptr<IBus<uint16_t, uint8_t>> m_cpuBus;
	std::shared_ptr<IBus<uint16_t, uint8_t>> m_ppuBus;

	std::shared_ptr<IRam<uint16_t, uint8_t>> m_ram;

	std::shared_ptr<IRam<uint16_t, uint8_t>> m_simpleRom;
	std::shared_ptr<NesCartridge> m_cartridge;

	std::shared_ptr<IRam<uint16_t, uint8_t>> m_patternTable;
	std::shared_ptr<IRam<uint16_t, uint8_t>> m_nameTable;
	std::shared_ptr<IRam<uint16_t, uint8_t>> m_palletteRam;

private:
	void runCPU_nCycles(size_t nCycles);
	void runCPU_nInstructions(size_t nInstructions);
	void runCPU_nCycles(size_t nCycles, uint16_t pc);
	void runCPU_nInstructions(size_t nInstructions, uint16_t pc);

private:
	bool isOn = false;
	size_t totalCyclesPassed = 0;

};
