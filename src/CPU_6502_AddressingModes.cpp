#include "fmt/format.h"
#include "fmt/printf.h"
#include "fmt/core.h"

#include "CPU_6502.h"

#define _DEBUG_LOG

#ifdef _DEBUG_LOG
void CPU_6502::log() {
	fmt::printf("\n%04X  %s  A:%02X X:%02X Y:%02X P:%02X SP:%02X PPU:%3d,%3d CYC:%d",
		PC-1, debugBuffer.data(), A, X, Y, PS.data, SP, 0, 0, totalCyclesPassed);
	
	if (m_debugFile.is_open()) {
		fmt::fprintf(m_debugFile, "\n%04X  %s  A:%02X X:%02X Y:%02X P:%02X SP:%02X PPU:%3d,%3d CYC:%d",
			PC - 1, debugBuffer.data(), A, X, Y, PS.data, SP, 0, 0, totalCyclesPassed);
	}

	debugBuffer.clear();
}
#endif

//	+-----------------------+
//	|	Addressing Modes	|
//	+-----------------------+

// Implied
uint8_t CPU_6502::IMP() {
	fetchedData = A;

#ifdef _DEBUG_LOG
	if (opcode == 0x4A || opcode == 0x0A || opcode == 0x6A || opcode == 0x2A) {
		fmt::format_to(debugBuffer, "{:02X}        {:s} A                         ",
			opcode, currentInstructionName);
	}
	else if (opcode == 0x1A || opcode == 0x3A || opcode == 0x5A || opcode == 0x7A ||
		opcode == 0xDA || opcode == 0xFA) {
		fmt::format_to(debugBuffer, "{:02X}       *{:s}                           ",
			opcode, currentInstructionName);
	}
	else {
		fmt::format_to(debugBuffer, "{:02X}        {:s}                           ",
			opcode, currentInstructionName);
	}
	log();
#endif

	return 0;
}

// Immediate
uint8_t CPU_6502::IMM() {
	addressAbsolute = PC;

#ifdef _DEBUG_LOG
	if (opcode == 0x80 || opcode == 0xEB) {
		fmt::format_to(debugBuffer, "{:02X} {:02X}    *{:s} #${:02X}                      ",
			opcode, read(addressAbsolute), currentInstructionName, read(addressAbsolute));
	}
	else {
		fmt::format_to(debugBuffer, "{:02X} {:02X}     {:s} #${:02X}                      ",
			opcode, read(addressAbsolute), currentInstructionName, read(addressAbsolute));
	}
	log();
#endif

	PC++;

	return 0;
}

// Zero Page
uint8_t CPU_6502::ZP0() {
	addressAbsolute = read(PC);
	addressAbsolute &= 0x00FF;

#ifdef _DEBUG_LOG
	if (opcode == 0x04 || opcode == 0x44 || opcode == 0x64 || opcode == 0xA7 || opcode == 0x87 ||
		opcode == 0xC7) {
	result = read(PC);

	fmt::format_to(debugBuffer, "{:02X} {:02X}    *{:s} ${:02X} = {:02X}                  ",
		opcode, result, currentInstructionName, result, read(result));
	}
	else {
		fmt::format_to(debugBuffer, "{:02X} {:02X}     {:s} ${:02X} = {:02X}                  ",
			opcode, addressAbsolute, currentInstructionName, addressAbsolute, read(addressAbsolute));
	}
	log();
#endif

	PC++;

	return 0;
}

// Zero Page - X Offset
uint8_t CPU_6502::ZPX() {
	addressAbsolute = (read(PC) + X);
	addressAbsolute &= 0x00FF;

#ifdef _DEBUG_LOG
	result = read(PC);
	if (opcode == 0x14 || opcode == 0x34 || opcode == 0x54 || opcode == 0x74 ||
		opcode == 0xD4 || opcode == 0xF4 || opcode == 0xD7) {
		fmt::format_to(debugBuffer, "{:02X} {:02X}    *{:s} ${:02X},X @ {:02X} = {:02X}        ",
			opcode, result, currentInstructionName, result, addressAbsolute, read(addressAbsolute));
	}
	else {
		fmt::format_to(debugBuffer, "{:02X} {:02X}     {:s} ${:02X},X @ {:02X} = {:02X}        ",
			opcode, result, currentInstructionName, result, addressAbsolute, read(addressAbsolute));
	}
	log();
#endif

	PC++;

	return 0;
}

// Zero Page - Y Offset
uint8_t CPU_6502::ZPY() {
	addressAbsolute = (read(PC) + Y);
	addressAbsolute &= 0x00FF;

#ifdef _DEBUG_LOG
	result = read(PC);

	if (opcode == 0xB7 || opcode == 0x97) {
		fmt::format_to(debugBuffer, "{:02X} {:02X}    *{:s} ${:02X},Y @ {:02X} = {:02X}        ",
			opcode, result, currentInstructionName, result, addressAbsolute, read(addressAbsolute));
	}
	else {
		fmt::format_to(debugBuffer, "{:02X} {:02X}     {:s} ${:02X},Y @ {:02X} = {:02X}        ",
			opcode, result, currentInstructionName, result, addressAbsolute, read(addressAbsolute));
	}
	log();
#endif

	PC++;

	return 0;
}

// Absolute
uint8_t CPU_6502::ABS() {
	uint16_t lo = read(PC);
	uint16_t hi = read(PC+1);
	addressAbsolute = (hi << 8) | lo;

#ifdef _DEBUG_LOG
	if (opcode ==  0x4C || opcode == 0x20) {
		fmt::format_to(debugBuffer, "{:02X} {:02X} {:02X}  {:s} ${:04X}                     ",
			opcode, lo, hi, currentInstructionName, addressAbsolute);
	}
	else if (opcode == 0x0C || opcode == 0xAF || opcode == 0x8F || opcode == 0xCF) {
		fmt::format_to(debugBuffer, "{:02X} {:02X} {:02X} *{:s} ${:04X} = {:02X}                ",
			opcode, lo, hi, currentInstructionName, addressAbsolute, read(addressAbsolute));
	}
	else {
		fmt::format_to(debugBuffer, "{:02X} {:02X} {:02X}  {:s} ${:04X} = {:02X}                ",
			opcode, lo, hi, currentInstructionName, addressAbsolute, read(addressAbsolute));
	}
	log();
#endif

	PC += 2;

	return 0;
}

// Absolute - X Offset
uint8_t CPU_6502::ABX() {
	uint16_t lo = read(PC);
	uint16_t hi = read(PC+1);
	addressAbsolute = (hi << 8) | lo;
	addressAbsolute += X;

#ifdef _DEBUG_LOG
	result = (hi << 8) | lo;

	if (opcode == 0x1C || opcode == 0x3C || opcode == 0x5C || opcode == 0x7C || opcode == 0xDC ||
		opcode == 0xFC || opcode == 0xDF) {
		fmt::format_to(debugBuffer, "{:02X} {:02X} {:02X} *{:s} ${:04X},X @ {:04X} = {:02X}       ",
			opcode, lo, hi, currentInstructionName, result,
			addressAbsolute, read(addressAbsolute));
	}
	else {
		fmt::format_to(debugBuffer, "{:02X} {:02X} {:02X}  {:s} ${:04X},X @ {:04X} = {:02X}       ",
			opcode, lo, hi, currentInstructionName, result,
			addressAbsolute, read(addressAbsolute));
	}
	log();
#endif

	PC += 2;

	if ((addressAbsolute & 0xFF00) != (hi << 8))
		return 1;
	else
		return 0;
}

// Absolute - Y Offset
uint8_t CPU_6502::ABY() {
	uint16_t lo = read(PC);
	uint16_t hi = read(PC + 1);
	addressAbsolute = (hi << 8) | lo;
	addressAbsolute += Y;

#ifdef _DEBUG_LOG
	result = (hi << 8) | lo;

	if (opcode == 0xBF || opcode == 0xDB) {
		fmt::format_to(debugBuffer, "{:02X} {:02X} {:02X} *{:s} ${:04X},Y @ {:04X} = {:02X}       ",
			opcode, lo, hi, currentInstructionName, result, addressAbsolute, read(addressAbsolute));
	}
	else {
		fmt::format_to(debugBuffer, "{:02X} {:02X} {:02X}  {:s} ${:04X},Y @ {:04X} = {:02X}       ",
			opcode, lo, hi, currentInstructionName, result, addressAbsolute, read(addressAbsolute));
	}
	log();
#endif

	PC += 2;

	if ((addressAbsolute & 0xFF00) != (hi << 8))
		return 1;
	else
		return 0;
}

// Indirect
uint8_t CPU_6502::IND() {
	uint16_t ptrLo = read(PC);
	uint16_t ptrHi = read(PC+1);
	uint16_t ptr = (ptrHi << 8) | ptrLo;


	if (ptrLo == 0x00FF) // Simulate page boundary hardware bug
	{
		addressAbsolute = (read(ptrHi << 8) << 8) | read(ptr);
	}
	else // Behave normally
	{
		addressAbsolute = (read(ptr + 1) << 8) | read(ptr);
	}

#ifdef _DEBUG_LOG
	fmt::format_to(debugBuffer, "{:02X} {:02X} {:02X}  {:s} (${:04X}) = {:04X}            ",
		opcode, ptrLo, ptrHi, currentInstructionName, ptr, addressAbsolute);
	log();
#endif

	PC += 2;

	return 0;
}

// Indirect - X Offset
uint8_t CPU_6502::IZX() {
	result = read(PC);
	uint16_t lo = read(lowByte((uint16_t)(result + (uint16_t)X)));
	uint16_t hi = read(lowByte((uint16_t)(result + (uint16_t)X + 1)));

	addressAbsolute = (hi << 8) | lo;

#ifdef _DEBUG_LOG
	if (opcode == 0xA3 || opcode == 0x83 || opcode == 0xC3) {
		fmt::format_to(debugBuffer, "{:02X} {:02X}    *{:s} (${:02X},X) @ {:02X} = {:04X} = {:02X}  ",
			opcode, result, currentInstructionName, result, lowByte(result + X), addressAbsolute, read(addressAbsolute));
	}
	else {
		fmt::format_to(debugBuffer, "{:02X} {:02X}     {:s} (${:02X},X) @ {:02X} = {:04X} = {:02X}  ",
			opcode, result, currentInstructionName, result, lowByte(result + X), addressAbsolute, read(addressAbsolute));
	}
	log();
#endif

	PC++;

	return 0;
}

// Indirect - Y Offset
uint8_t CPU_6502::IZY() {
	result = read(PC);
	uint16_t lo = read(lowByte(result));
	uint16_t hi = read(lowByte(result + 1));

	addressAbsolute = (hi << 8) | lo;

#ifdef _DEBUG_LOG
	if (opcode == 0xA3 || opcode == 0xB3 || opcode == 0xD3) {
		fmt::format_to(debugBuffer, "{:02X} {:02X}    *{:s} (${:02X}),Y = {:04X} @ {:04X} = {:02X}",
			opcode, result, currentInstructionName, result, addressAbsolute, (addressAbsolute + Y) & 0xFFFF, read((addressAbsolute + Y) & 0xFFFF));
	}
	else {
		fmt::format_to(debugBuffer, "{:02X} {:02X}     {:s} (${:02X}),Y = {:04X} @ {:04X} = {:02X}",
			opcode, result, currentInstructionName, result, addressAbsolute, (addressAbsolute + Y) & 0xFFFF, read((addressAbsolute + Y) & 0xFFFF));
	}
	log();
#endif

	addressAbsolute += Y;

	PC++;

	if ((addressAbsolute & 0xFF00) != (hi << 8))
		return 1;
	else
		return 0;

	return 0;
}

// Relative
uint8_t CPU_6502::REL() {
	addressRelative = read(PC);

	if (addressRelative & 0x80)
		addressRelative |= 0xFF00;

#ifdef _DEBUG_LOG
	fmt::format_to(debugBuffer, "{:02X} {:02X}     {:s} ${:04X}                     ",
		opcode, read(PC), currentInstructionName, (PC+1 + addressRelative) & 0xFFFF);
	log();
#endif

	PC++;

	return 0;
}
