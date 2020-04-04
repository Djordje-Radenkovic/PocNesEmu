#pragma once

#include "Config.h"
#include "INesCartridge.h"
#include "INesCpu.h"
#include "INesPpu.h"
#include "INesPpu.h"
#include "IRam.h"
#include "NesArrayRam.h"


class NesCore final {
private:
	INesCpu* m_cpu;
	INesPpu* m_ppu;
	IRam<uint16_t, uint8_t>* m_ram;
	Bus<uint16_t, uint8_t>* m_cpuBus;
	Bus<uint16_t, uint8_t>* m_ppuBus;
	IRam<uint16_t, uint8_t>* m_patternTable;
	IRam<uint16_t, uint8_t>* m_nameTable;
	IRam<uint16_t, uint8_t>* m_palletteRam;
	// ...

	void runCPU_nCycles(size_t nCycles);
	void runCPU_nInstructions(size_t nInstructions);
	static void dump_memory(Bus<uint16_t, uint8_t>* bus, size_t startAddress, size_t endAddress);

public:
	NesCore(INesCpu* cpu, INesPpu* ppu, IRam<uint16_t, uint8_t>* ram,
		Bus<uint16_t, uint8_t>* cpuBus, Bus<uint16_t, uint8_t>* ppuBus);
	~NesCore();

	void nesTest();

	void powerOn();
	void powerOff();
	void pause();
	void resume();
	void saveState();
	void loadState();
	bool loadCartridge(const char* filePath);
	// ...
};
