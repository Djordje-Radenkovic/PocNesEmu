#pragma once

#include "INesCartridge.h"
#include "INesCpu.h"
#include "INesPpu.h"
#include "IRam.h"


class NesCore {
private:
	INesCpu* m_cpu;
	IRam<uint16_t, uint8_t>* m_ram;
	// ...

public:
	NesCore(INesCpu* cpu, IRam<uint16_t, uint8_t>* ram)
		: m_cpu(cpu), m_ram(ram) {}

	~NesCore() {
		m_cpu->~INesCpu();
		m_ram->~IRam();
	}

	void power_on();
	void power_off();
	void pause();
	void resume();
	void save_state();
	void load_state();
	void load_cartridge();
	// ...
};
