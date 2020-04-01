#include <iostream>
#include <fstream>

#include "NesCore.h"
#include "CPU_6502.h"
#include "NesArrayRam.h"
#include "Bus.h"

#define DEBUG_FILE_PATH "C:\\Users\\Anic\\Desktop\\cpu.log"
#define ROM_FILE_PATH "C:\\Users\\Anic\\Desktop\\nestest.nes"
#define MEM_DUMP_FILE_PATH "C:\\Users\\Anic\\Desktop\\dump.mem"


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


int main(int argc, char** argv) {
	// Create debug output file
	FILE* debugFile;
	fopen_s(&debugFile, DEBUG_FILE_PATH, "w");

	// Create system components
	INesCpu* cpu = new CPU_6502(debugFile);
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
		std::cout << "Couldn't open nestest.nes file!" << std::endl;

		return 1;
	}

	size = nesTestRom.tellg();
	memblock = new uint8_t[size];
	nesTestRom.seekg(0, std::ios::beg);
	nesTestRom.read((char*)memblock, size);
	const int romSize = nesTestRom.gcount();
	nesTestRom.close();

	std::cout << "NESTest ROM read." << std::endl;

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
	std::cout << "NESTest ROM loaded into memory." << std::endl;


	// Debug memory dump
	const uint16_t dumpStartAddress = 0x0000;
	const uint16_t dumpEndAddress =	  0xFFFF;

	FILE* dbg;
	fopen_s(&dbg, MEM_DUMP_FILE_PATH, "w");
	for (int i = 0; i <= 0xFFFF; i++) {
		if (i % 0x10 == 0) {
			fprintf(dbg, "\n0x%04X: ", i);
		}
		fprintf_s(dbg, "%02X ", bus->read(i,false, false));
	}
	fflush(dbg);
	printf_s("Dumped memory to disk ($%04X-$%04X).\n", dumpStartAddress, dumpEndAddress);


	// Write dummy reset vector
	bus->write(0xFFFC, 0x00);
	bus->write(0xFFFD, 0xC0);

	// Run the CPU
	runCPU_nCycles(cpu, 26554);

	fflush(debugFile);

	return 0;
}
