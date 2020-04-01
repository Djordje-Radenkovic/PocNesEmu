#include <fstream>

#include "fmt/printf.h"

#include "Config.h"
#include "filesystem.h"
#include "NesCore.h"
#include "CPU_6502.h"
#include "NesArrayRam.h"
#include "Bus.h"


void runCPU_nCycles(INesCpu* cpu,size_t numCycles) {
	cpu->reset();
	do {
		cpu->tick();
	} while (cpu->getCyclesPassed() < numCycles);
}

void runCPU_nInstructions(INesCpu* cpu, size_t numInstructions) {
	cpu->reset();
	do {
		cpu->tick();
		if (cpu->isFinished())
			numInstructions--;
	} while (numInstructions > 0);
}

void dump_memory(Bus<uint16_t, uint8_t>* bus, size_t startAddress = 0, size_t endAddress = 0xFFFF) {	
	std::ofstream memDumpFile(MEM_DUMP_FILE_PATH, std::ofstream::out);
	if (!memDumpFile.is_open()) {
		fmt::print("Failed to open memdump.log file!\n");
		return;
	}

	for (int i = 0; i <= 0xFFFF; i++) {
		if (i % 0x10 == 0) {
			fmt::fprintf(memDumpFile, "\n0x%04X: ", i);
		}
		fmt::fprintf(memDumpFile, "%02X ", bus->read(i, false, false));
	}
	memDumpFile.flush();
	memDumpFile.close();
	fmt::printf("Dumped memory to disk ($%04X-$%04X).\n", startAddress, endAddress);
}

int main(int argc, char** argv) {
	// Create logs folder
	if (!POCNES::dirExists(LOGS_FOLDER_PATH))
		POCNES::makedir(LOGS_FOLDER_PATH);


	// Create system components
	INesCpu* cpu = new CPU_6502(DEBUG_FILE_PATH);
	IRam<uint16_t, uint8_t>* mainMemory = new NesArrayRam(0x0800);
	IRam<uint16_t, uint8_t>* mainRom;
	IRam<uint16_t, uint8_t>* otherRom;


	// Create and set up the system bus
	auto* bus = new Bus<uint16_t, uint8_t>();
	cpu->connectBus(bus);
	bus->addSlave(mainMemory, 0x0000);
	bus->addSlave(mainMemory, 0x0800);
	bus->addSlave(mainMemory, 0x1000);
	bus->addSlave(mainMemory, 0x1800);


	// Load test ROM
	std::streampos size;
	uint8_t* memblock = nullptr;

	std::ifstream nesTestRom(ROM_FILE_PATH,
		std::ios::binary | std::ios::in | std::ios::ate);

	if (!nesTestRom.is_open()) {
		fmt::print("Couldn't open nestest.nes file!\n");;

		return 1;
	}

	size = nesTestRom.tellg();
	memblock = new uint8_t[size];
	nesTestRom.seekg(0, std::ios::beg);
	nesTestRom.read((char*)memblock, size);
	const int romSize = nesTestRom.gcount();
	nesTestRom.close();

	fmt::print("NESTest ROM read.\n");

	const uint16_t baseAddress = 0x8000;
	const uint16_t otherAddress = 0xC000;
	const uint16_t romOffset = 0x0010;

	mainRom = new NesArrayRam(0x4000);
	otherRom = new NesArrayRam(0x4000);
	bus->addSlave(mainRom, baseAddress);
	bus->addSlave(otherRom, otherAddress);

	for (int i = 0; i < 0x4000; i++) {
		bus->write(baseAddress + i, memblock[romOffset + i]);
		bus->write(otherAddress + i, memblock[romOffset + i]);
	}
	fmt::print("NESTest ROM loaded into memory.\n");


	// Debug memory dump
	dump_memory(bus);

	
	// Write dummy reset vector
	bus->write(0xFFFC, 0x00);
	bus->write(0xFFFD, 0xC0);




	// Run the CPU
	// runCPU_nInstructions(cpu, 5);
	runCPU_nCycles(cpu, 26554);		// NESTest runs for 26554 cycles


	return 0;
}
