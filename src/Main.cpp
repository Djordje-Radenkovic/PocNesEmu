#include <memory>

#include "sdl/SDL.h"

#include "Config.h"
#include "filesystem.h"
#include "NesCore.h"
#include "CPU_6502.h"
#include "PPU_2C02.h"
#include "NesArrayRam.h"
#include "NesMultiMapBus.h"


int main(int argc, char** argv) {
	// Create logs folder
	if (!POCNES::dirExists(LOGS_FOLDER_PATH))
		POCNES::makedir(LOGS_FOLDER_PATH);

	// Create the system instance
	NesCore nes(
		std::make_shared<CPU_6502>(),
		std::make_shared<PPU_2C02>(),
		std::make_shared<NesArrayRam>(0x0800),
		std::make_shared<NesMultiMapBus>(),
		std::make_shared<NesMultiMapBus>()
	);

	//nes.nesTest(NESTEST_FILE_PATH, MEM_DUMP_FILE_PATH);

	nes.loadCartridge("./roms/games/dk.nes");
	nes.powerOn();


	return 0;
}
