#include "Config.h"
#include "filesystem.h"
#include "NesCore.h"
#include "CPU_6502.h"
#include "PPU_2C02.h"
#include "NesArrayRam.h"
#include "NesMultiMapBus.h"
#include "NesVectorBus.h"


int main(int argc, char** argv) {
	// Create logs folder
	if (!POCNES::dirExists(LOGS_FOLDER_PATH))
		POCNES::makedir(LOGS_FOLDER_PATH);

	// Create system components
	INesCpu* cpu = new CPU_6502(DEBUG_FILE_PATH);
	INesPpu* ppu = new PPU_2C02();
	IRam<uint16_t, uint8_t>* mainMemory = new NesArrayRam(0x0800);
	IBus<uint16_t, uint8_t>* cpuBus = new NesMultiMapBus;
	IBus<uint16_t, uint8_t>* ppuBus = new NesMultiMapBus;

	// Create the system instance
	NesCore nes(cpu, ppu, mainMemory, cpuBus, ppuBus);

	// Run NesTest
	nes.nesTest();

	return 0;
}
