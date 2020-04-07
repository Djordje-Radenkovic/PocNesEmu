#include "CPU_6502.h"


CPU_6502::CPU_6502(const char* debugFilePath) {
	m_debugFile.open(debugFilePath, std::ofstream::out);
	
	if (!m_debugFile.is_open()) {
		fmt::print("Failed to open cpu.log file!\n");
	}
}

CPU_6502::~CPU_6502() {
	m_debugFile.close();
}

struct CPU_6502::CpuInstruction
{
	std::string name;
	uint8_t(CPU_6502::* operation)(void) = nullptr;
	uint8_t(CPU_6502::* addressMode)(void) = nullptr;
	uint8_t cycles = 0;
};

void CPU_6502::connectBus(std::shared_ptr<IBus<uint16_t, uint8_t>> bus) {
	m_bus = bus;
}

bool CPU_6502::isIMP() {
	return (lookup[opcode].addressMode == &CPU_6502::IMP);
}

bool CPU_6502::isIMM() {
	return (lookup[opcode].addressMode == &CPU_6502::IMM);
}

uint8_t CPU_6502::fetchData() {
	if (!isIMP())
		fetchedData = readFrom(addressAbsolute);

	return fetchedData;
}

// CPU Reset Function
void CPU_6502::reset() {
	size_t totalCyclesPassed = 0;

	// Get address for start of execution
	// and set the Program Counter to it
	addressAbsolute = 0xFFFC;
	uint16_t lo = readFrom(addressAbsolute);
	uint16_t hi = readFrom(addressAbsolute + 1);
	PC = (hi << 8) | lo;

	// Reset registers to known state
	A = 0;	X = 0;	Y = 0;
	SP = 0xFD;
	PS.data = 0x24;

	// Reset helper variables
	addressAbsolute = 0x0000;
	addressRelative = 0x0000;
	fetchedData = 0x00;

	// Reset takes 7 cycles
	cycles = 7;
}

void CPU_6502::reset(uint16_t pc) {
	size_t totalCyclesPassed = 0;

	// Set Program Counter to reset address
	PC = pc;

	// Reset registers to known state
	A = 0;	X = 0;	Y = 0;
	SP = 0xFD;
	PS.data = 0x24;

	// Reset helper variables
	addressAbsolute = 0x0000;
	addressRelative = 0x0000;
	fetchedData = 0x00;

	// Reset takes 7 cycles
	cycles = 7;
}


// Interrupt Request
void CPU_6502::irq() {
	if (PS.ID == 0) {
		push(highByte(PC));
		push(lowByte(PC));

		PS.BC = 1;
		PS.ID = 1;
		push(PS.data);

		// readFrom new program counter location from fixed address
		uint16_t lo = readFrom(irqVectorLow);
		uint16_t hi = readFrom(irqVectorHigh);
		PC = (hi << 8) | lo;

		// IRQ taks 7 cycles
		cycles = 7;
	}
}

// Non-Maskable Interrupt
void CPU_6502::nmi() {
	push(highByte(PC));
	push(lowByte(PC));

	PS.BC = 1;
	PS.ID = 1;
	push(PS.data);

	uint16_t lo = readFrom(nmiVectorLow);
	uint16_t hi = readFrom(nmiVectorHigh);
	PC = (hi << 8) | lo;

	// NMI taks 8 cycles
	cycles = 8;
}

// Runs every clock cycle
void CPU_6502::tick() {
	if (cycles == 0) {
		opcode = readFrom(PC++);
		
		PS.XX = 1;

		currentInstructionName = (this->lookup[opcode].name);

		cycles = lookup[opcode].cycles;

		uint8_t additionalCycle1 = (this->*lookup[opcode].addressMode)();
		uint8_t additionalCycle2 = (this->*lookup[opcode].operation)();

		cycles += (additionalCycle1 & additionalCycle2);

		PS.XX = 1;
	}

	totalCyclesPassed++;
	cycles--;
}

bool CPU_6502::isFinished() {
	return cycles == 0;
}

//	+-----------------------+
//	|	  Lookup Vector		|
//	+-----------------------+

using c = CPU_6502;

std::vector<CPU_6502::CpuInstruction> CPU_6502::lookup =
{
//			|               x0            |               x1             |               x2             |               x3             |               x4             |               x5             |               x6             |               x7             |               x8             |               x9             |               xA             |               xB             |               xC             |               xD             |               xE             |               xF             |
/*  0x  */	{ "BRK", &c::BRK, &c::IMM, 7 },{ "ORA", &c::ORA, &c::IZX, 6 },{ "???", &c::XXX, &c::IMP, 2 },{ "SLO", &c::SLO, &c::IZX, 8 },{ "NOP", &c::NOP, &c::ZP0, 3 },{ "ORA", &c::ORA, &c::ZP0, 3 },{ "ASL", &c::ASL, &c::ZP0, 5 },{ "SLO", &c::SLO, &c::ZP0, 5 },{ "PHP", &c::PHP, &c::IMP, 3 },{ "ORA", &c::ORA, &c::IMM, 2 },{ "ASL", &c::ASL, &c::IMP, 2 },{ "???", &c::XXX, &c::IMP, 2 },{ "NOP", &c::NOP, &c::ABS, 4 },{ "ORA", &c::ORA, &c::ABS, 4 },{ "ASL", &c::ASL, &c::ABS, 6 },{ "SLO", &c::SLO, &c::ABS, 6 },
/*  1x  */	{ "BPL", &c::BPL, &c::REL, 2 },{ "ORA", &c::ORA, &c::IZY, 5 },{ "???", &c::XXX, &c::IMP, 2 },{ "SLO", &c::SLO, &c::IZY, 8 },{ "NOP", &c::NOP, &c::ZPX, 4 },{ "ORA", &c::ORA, &c::ZPX, 4 },{ "ASL", &c::ASL, &c::ZPX, 6 },{ "SLO", &c::SLO, &c::ZPX, 6 },{ "CLC", &c::CLC, &c::IMP, 2 },{ "ORA", &c::ORA, &c::ABY, 4 },{ "NOP", &c::NOP, &c::IMP, 2 },{ "SLO", &c::SLO, &c::ABY, 7 },{ "NOP", &c::NOP, &c::ABX, 4 },{ "ORA", &c::ORA, &c::ABX, 4 },{ "ASL", &c::ASL, &c::ABX, 7 },{ "SLO", &c::SLO, &c::ABX, 7 },
/*  2x  */	{ "JSR", &c::JSR, &c::ABS, 6 },{ "AND", &c::AND, &c::IZX, 6 },{ "???", &c::XXX, &c::IMP, 2 },{ "RLA", &c::RLA, &c::IZX, 8 },{ "BIT", &c::BIT, &c::ZP0, 3 },{ "AND", &c::AND, &c::ZP0, 3 },{ "ROL", &c::ROL, &c::ZP0, 5 },{ "RLA", &c::RLA, &c::ZP0, 5 },{ "PLP", &c::PLP, &c::IMP, 4 },{ "AND", &c::AND, &c::IMM, 2 },{ "ROL", &c::ROL, &c::IMP, 2 },{ "???", &c::XXX, &c::IMP, 2 },{ "BIT", &c::BIT, &c::ABS, 4 },{ "AND", &c::AND, &c::ABS, 4 },{ "ROL", &c::ROL, &c::ABS, 6 },{ "RLA", &c::RLA, &c::ABS, 6 },
/*  3x  */	{ "BMI", &c::BMI, &c::REL, 2 },{ "AND", &c::AND, &c::IZY, 5 },{ "???", &c::XXX, &c::IMP, 2 },{ "RLA", &c::RLA, &c::IZY, 8 },{ "NOP", &c::NOP, &c::ZPX, 4 },{ "AND", &c::AND, &c::ZPX, 4 },{ "ROL", &c::ROL, &c::ZPX, 6 },{ "RLA", &c::RLA, &c::ZPX, 6 },{ "SEC", &c::SEC, &c::IMP, 2 },{ "AND", &c::AND, &c::ABY, 4 },{ "NOP", &c::NOP, &c::IMP, 2 },{ "RLA", &c::RLA, &c::ABY, 7 },{ "NOP", &c::NOP, &c::ABX, 4 },{ "AND", &c::AND, &c::ABX, 4 },{ "ROL", &c::ROL, &c::ABX, 7 },{ "RLA", &c::RLA, &c::ABX, 7 },
/*  4x  */	{ "RTI", &c::RTI, &c::IMP, 6 },{ "EOR", &c::EOR, &c::IZX, 6 },{ "???", &c::XXX, &c::IMP, 2 },{ "SRE", &c::SRE, &c::IZX, 8 },{ "NOP", &c::NOP, &c::ZP0, 3 },{ "EOR", &c::EOR, &c::ZP0, 3 },{ "LSR", &c::LSR, &c::ZP0, 5 },{ "SRE", &c::SRE, &c::ZP0, 5 },{ "PHA", &c::PHA, &c::IMP, 3 },{ "EOR", &c::EOR, &c::IMM, 2 },{ "LSR", &c::LSR, &c::IMP, 2 },{ "???", &c::XXX, &c::IMP, 2 },{ "JMP", &c::JMP, &c::ABS, 3 },{ "EOR", &c::EOR, &c::ABS, 4 },{ "LSR", &c::LSR, &c::ABS, 6 },{ "SRE", &c::SRE, &c::ABS, 6 },
/*  5x  */	{ "BVC", &c::BVC, &c::REL, 2 },{ "EOR", &c::EOR, &c::IZY, 5 },{ "???", &c::XXX, &c::IMP, 2 },{ "SRE", &c::SRE, &c::IZY, 8 },{ "NOP", &c::NOP, &c::ZPX, 4 },{ "EOR", &c::EOR, &c::ZPX, 4 },{ "LSR", &c::LSR, &c::ZPX, 6 },{ "SRE", &c::SRE, &c::ZPX, 6 },{ "CLI", &c::CLI, &c::IMP, 2 },{ "EOR", &c::EOR, &c::ABY, 4 },{ "NOP", &c::NOP, &c::IMP, 2 },{ "SRE", &c::SRE, &c::ABY, 7 },{ "NOP", &c::NOP, &c::ABX, 4 },{ "EOR", &c::EOR, &c::ABX, 4 },{ "LSR", &c::LSR, &c::ABX, 7 },{ "SRE", &c::SRE, &c::ABX, 7 },
/*  6x  */	{ "RTS", &c::RTS, &c::IMP, 6 },{ "ADC", &c::ADC, &c::IZX, 6 },{ "???", &c::XXX, &c::IMP, 2 },{ "RRA", &c::RRA, &c::IZX, 8 },{ "NOP", &c::NOP, &c::ZP0, 3 },{ "ADC", &c::ADC, &c::ZP0, 3 },{ "ROR", &c::ROR, &c::ZP0, 5 },{ "RRA", &c::RRA, &c::ZP0, 5 },{ "PLA", &c::PLA, &c::IMP, 4 },{ "ADC", &c::ADC, &c::IMM, 2 },{ "ROR", &c::ROR, &c::IMP, 2 },{ "???", &c::XXX, &c::IMP, 2 },{ "JMP", &c::JMP, &c::IND, 5 },{ "ADC", &c::ADC, &c::ABS, 4 },{ "ROR", &c::ROR, &c::ABS, 6 },{ "RRA", &c::RRA, &c::ABS, 6 },
/*  7x  */	{ "BVS", &c::BVS, &c::REL, 2 },{ "ADC", &c::ADC, &c::IZY, 5 },{ "???", &c::XXX, &c::IMP, 2 },{ "RRA", &c::RRA, &c::IZY, 8 },{ "NOP", &c::NOP, &c::ZPX, 4 },{ "ADC", &c::ADC, &c::ZPX, 4 },{ "ROR", &c::ROR, &c::ZPX, 6 },{ "RRA", &c::RRA, &c::ZPX, 6 },{ "SEI", &c::SEI, &c::IMP, 2 },{ "ADC", &c::ADC, &c::ABY, 4 },{ "NOP", &c::NOP, &c::IMP, 2 },{ "RRA", &c::RRA, &c::ABY, 7 },{ "NOP", &c::NOP, &c::ABX, 4 },{ "ADC", &c::ADC, &c::ABX, 4 },{ "ROR", &c::ROR, &c::ABX, 7 },{ "RRA", &c::RRA, &c::ABX, 7 },
/*  8x  */	{ "NOP", &c::NOP, &c::IMM, 2 },{ "STA", &c::STA, &c::IZX, 6 },{ "???", &c::NOP, &c::IMP, 2 },{ "SAX", &c::SAX, &c::IZX, 6 },{ "STY", &c::STY, &c::ZP0, 3 },{ "STA", &c::STA, &c::ZP0, 3 },{ "STX", &c::STX, &c::ZP0, 3 },{ "SAX", &c::SAX, &c::ZP0, 3 },{ "DEY", &c::DEY, &c::IMP, 2 },{ "???", &c::NOP, &c::IMP, 2 },{ "TXA", &c::TXA, &c::IMP, 2 },{ "???", &c::XXX, &c::IMP, 2 },{ "STY", &c::STY, &c::ABS, 4 },{ "STA", &c::STA, &c::ABS, 4 },{ "STX", &c::STX, &c::ABS, 4 },{ "SAX", &c::SAX, &c::ABS, 4 },
/*  9x  */	{ "BCC", &c::BCC, &c::REL, 2 },{ "STA", &c::STA, &c::IZY, 6 },{ "???", &c::XXX, &c::IMP, 2 },{ "???", &c::XXX, &c::IMP, 6 },{ "STY", &c::STY, &c::ZPX, 4 },{ "STA", &c::STA, &c::ZPX, 4 },{ "STX", &c::STX, &c::ZPY, 4 },{ "SAX", &c::SAX, &c::ZPY, 4 },{ "TYA", &c::TYA, &c::IMP, 2 },{ "STA", &c::STA, &c::ABY, 5 },{ "TXS", &c::TXS, &c::IMP, 2 },{ "???", &c::XXX, &c::IMP, 5 },{ "NOP", &c::NOP, &c::ABX, 5 },{ "STA", &c::STA, &c::ABX, 5 },{ "???", &c::XXX, &c::IMP, 5 },{ "???", &c::XXX, &c::IMP, 5 },
/*  Ax  */	{ "LDY", &c::LDY, &c::IMM, 2 },{ "LDA", &c::LDA, &c::IZX, 6 },{ "LDX", &c::LDX, &c::IMM, 2 },{ "LAX", &c::LAX, &c::IZX, 6 },{ "LDY", &c::LDY, &c::ZP0, 3 },{ "LDA", &c::LDA, &c::ZP0, 3 },{ "LDX", &c::LDX, &c::ZP0, 3 },{ "LAX", &c::LAX, &c::ZP0, 3 },{ "TAY", &c::TAY, &c::IMP, 2 },{ "LDA", &c::LDA, &c::IMM, 2 },{ "TAX", &c::TAX, &c::IMP, 2 },{ "???", &c::XXX, &c::IMP, 2 },{ "LDY", &c::LDY, &c::ABS, 4 },{ "LDA", &c::LDA, &c::ABS, 4 },{ "LDX", &c::LDX, &c::ABS, 4 },{ "LAX", &c::LAX, &c::ABS, 4 },
/*  Bx  */	{ "BCS", &c::BCS, &c::REL, 2 },{ "LDA", &c::LDA, &c::IZY, 5 },{ "???", &c::XXX, &c::IMP, 2 },{ "LAX", &c::LAX, &c::IZY, 5 },{ "LDY", &c::LDY, &c::ZPX, 4 },{ "LDA", &c::LDA, &c::ZPX, 4 },{ "LDX", &c::LDX, &c::ZPY, 4 },{ "LAX", &c::LAX, &c::ZPY, 4 },{ "CLV", &c::CLV, &c::IMP, 2 },{ "LDA", &c::LDA, &c::ABY, 4 },{ "TSX", &c::TSX, &c::IMP, 2 },{ "???", &c::XXX, &c::IMP, 4 },{ "LDY", &c::LDY, &c::ABX, 4 },{ "LDA", &c::LDA, &c::ABX, 4 },{ "LDX", &c::LDX, &c::ABY, 4 },{ "LAX", &c::LAX, &c::ABY, 4 },
/*  Cx  */	{ "CPY", &c::CPY, &c::IMM, 2 },{ "CMP", &c::CMP, &c::IZX, 6 },{ "???", &c::NOP, &c::IMP, 2 },{ "DCP", &c::DCP, &c::IZX, 8 },{ "CPY", &c::CPY, &c::ZP0, 3 },{ "CMP", &c::CMP, &c::ZP0, 3 },{ "DEC", &c::DEC, &c::ZP0, 5 },{ "DCP", &c::DCP, &c::ZP0, 5 },{ "INY", &c::INY, &c::IMP, 2 },{ "CMP", &c::CMP, &c::IMM, 2 },{ "DEX", &c::DEX, &c::IMP, 2 },{ "???", &c::XXX, &c::IMP, 2 },{ "CPY", &c::CPY, &c::ABS, 4 },{ "CMP", &c::CMP, &c::ABS, 4 },{ "DEC", &c::DEC, &c::ABS, 6 },{ "DCP", &c::DCP, &c::ABS, 6 },
/*  Dx  */	{ "BNE", &c::BNE, &c::REL, 2 },{ "CMP", &c::CMP, &c::IZY, 5 },{ "???", &c::XXX, &c::IMP, 2 },{ "DCP", &c::DCP, &c::IZY, 8 },{ "NOP", &c::NOP, &c::ZPX, 4 },{ "CMP", &c::CMP, &c::ZPX, 4 },{ "DEC", &c::DEC, &c::ZPX, 6 },{ "DCP", &c::DCP, &c::ZPX, 6 },{ "CLD", &c::CLD, &c::IMP, 2 },{ "CMP", &c::CMP, &c::ABY, 4 },{ "NOP", &c::NOP, &c::IMP, 2 },{ "DCP", &c::DCP, &c::ABY, 7 },{ "NOP", &c::NOP, &c::ABX, 4 },{ "CMP", &c::CMP, &c::ABX, 4 },{ "DEC", &c::DEC, &c::ABX, 7 },{ "DCP", &c::DCP, &c::ABX, 7 },
/*  Ex  */	{ "CPX", &c::CPX, &c::IMM, 2 },{ "SBC", &c::SBC, &c::IZX, 6 },{ "???", &c::NOP, &c::IMP, 2 },{ "ISB", &c::ISB, &c::IZX, 8 },{ "CPX", &c::CPX, &c::ZP0, 3 },{ "SBC", &c::SBC, &c::ZP0, 3 },{ "INC", &c::INC, &c::ZP0, 5 },{ "ISB", &c::ISB, &c::ZP0, 5 },{ "INX", &c::INX, &c::IMP, 2 },{ "SBC", &c::SBC, &c::IMM, 2 },{ "NOP", &c::NOP, &c::IMP, 2 },{ "SBC", &c::SBC, &c::IMM, 2 },{ "CPX", &c::CPX, &c::ABS, 4 },{ "SBC", &c::SBC, &c::ABS, 4 },{ "INC", &c::INC, &c::ABS, 6 },{ "ISB", &c::ISB, &c::ABS, 6 },
/*  Fx  */	{ "BEQ", &c::BEQ, &c::REL, 2 },{ "SBC", &c::SBC, &c::IZY, 5 },{ "???", &c::XXX, &c::IMP, 2 },{ "ISB", &c::ISB, &c::IZY, 8 },{ "NOP", &c::NOP, &c::ZPX, 4 },{ "SBC", &c::SBC, &c::ZPX, 4 },{ "INC", &c::INC, &c::ZPX, 6 },{ "ISB", &c::ISB, &c::ZPX, 6 },{ "SED", &c::SED, &c::IMP, 2 },{ "SBC", &c::SBC, &c::ABY, 4 },{ "NOP", &c::NOP, &c::IMP, 2 },{ "ISB", &c::ISB, &c::ABY, 7 },{ "NOP", &c::NOP, &c::ABX, 4 },{ "SBC", &c::SBC, &c::ABX, 4 },{ "INC", &c::INC, &c::ABX, 7 },{ "ISB", &c::ISB, &c::ABX, 7 },
};
