#pragma once

#include "INesCartridge.h"
#include "INesCpu.h"
#include "INesPpu.h"
#include "IBus.h"
#include "IRam.h"
#include "NesArrayRam.h"


class NesCore final {
private:
	INesCpu* m_cpu;
	INesPpu* m_ppu;
	IRam<uint16_t, uint8_t>* m_ram;
	IBus<uint16_t, uint8_t>* m_cpuBus;
	IBus<uint16_t, uint8_t>* m_ppuBus;
	IRam<uint16_t, uint8_t>* m_patternTable;
	IRam<uint16_t, uint8_t>* m_nameTable;
	IRam<uint16_t, uint8_t>* m_palletteRam;
	// ...

	void runCPU_nCycles(size_t nCycles);
	void runCPU_nInstructions(size_t nInstructions);
	static void dump_memory(IBus<uint16_t, uint8_t>* bus, size_t startAddress, size_t endAddress);

public:
	NesCore(INesCpu* cpu, INesPpu* ppu, IRam<uint16_t, uint8_t>* ram,
		IBus<uint16_t, uint8_t>* cpuBus, IBus<uint16_t, uint8_t>* ppuBus);
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
