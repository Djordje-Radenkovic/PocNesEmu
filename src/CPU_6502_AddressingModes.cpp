#include "fmt/format.h"
#include "fmt/printf.h"
#include "fmt/core.h"

#include "CPU_6502.h"
#include "Config.h"


void CPU_6502::log() {
	fmt::format_to(logBuffer, "\n{:04X}  {:s}  A:{:02X} X:{:02X} Y:{:02X} P:{:02X} SP:{:02X} PPU:{:3d},{:3d} CYC:{:d}",
		PC-1, fmt::to_string(debugBuffer), A, X, Y, PS.data, SP, 0, 0, totalCyclesPassed);

	debugBuffer.clear();
}

std::string CPU_6502::getLog() {
	std::string temp = fmt::to_string(logBuffer);
	logBuffer.clear();

	return temp;
}


//	+-----------------------+
//	|	Addressing Modes	|
//	+-----------------------+

// Implied
uint8_t CPU_6502::IMP() {
	fetchedData = A;

#ifdef _LOG
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

#ifdef _LOG
	if (opcode == 0x80 || opcode == 0xEB) {
		fmt::format_to(debugBuffer, "{:02X} {:02X}    *{:s} #${:02X}                      ",
			opcode, readFrom(addressAbsolute, true), currentInstructionName, readFrom(addressAbsolute, true));
	}
	else {
		fmt::format_to(debugBuffer, "{:02X} {:02X}     {:s} #${:02X}                      ",
			opcode, readFrom(addressAbsolute, true), currentInstructionName, readFrom(addressAbsolute, true));
	}
	log();
#endif

	PC++;

	return 0;
}

// Zero Page
uint8_t CPU_6502::ZP0() {
	addressAbsolute = readFrom(PC);
	addressAbsolute &= 0x00FF;

#ifdef _LOG
	if (opcode == 0x04 || opcode == 0x44 || opcode == 0x64 || opcode == 0xA7 || opcode == 0x87 ||
		opcode == 0xC7 || opcode == 0xE7 || opcode == 0x07 || opcode == 0x27 || opcode == 0x47 ||
		opcode == 0x67) {
	result = readFrom(PC, true);

	fmt::format_to(debugBuffer, "{:02X} {:02X}    *{:s} ${:02X} = {:02X}                  ",
		opcode, result, currentInstructionName, result, readFrom(result, true));
	}
	else {
		fmt::format_to(debugBuffer, "{:02X} {:02X}     {:s} ${:02X} = {:02X}                  ",
			opcode, addressAbsolute, currentInstructionName, addressAbsolute, readFrom(addressAbsolute, true));
	}
	log();
#endif

	PC++;

	return 0;
}

// Zero Page - X Offset
uint8_t CPU_6502::ZPX() {
	addressAbsolute = (readFrom(PC) + X);
	addressAbsolute &= 0x00FF;

#ifdef _LOG
	result = readFrom(PC, true);
	if (opcode == 0x14 || opcode == 0x34 || opcode == 0x54 || opcode == 0x74 ||
		opcode == 0xD4 || opcode == 0xF4 || opcode == 0xD7 || opcode == 0xF7 ||
		opcode == 0x17 || opcode == 0x37 || opcode == 0x57 || opcode == 0x77) {
		fmt::format_to(debugBuffer, "{:02X} {:02X}    *{:s} ${:02X},X @ {:02X} = {:02X}           ",
			opcode, result, currentInstructionName, result, addressAbsolute, readFrom(addressAbsolute, true));
	}
	else {
		fmt::format_to(debugBuffer, "{:02X} {:02X}     {:s} ${:02X},X @ {:02X} = {:02X}           ",
			opcode, result, currentInstructionName, result, addressAbsolute, readFrom(addressAbsolute, true));
	}
	log();
#endif

	PC++;

	return 0;
}

// Zero Page - Y Offset
uint8_t CPU_6502::ZPY() {
	addressAbsolute = (readFrom(PC) + Y);
	addressAbsolute &= 0x00FF;

#ifdef _LOG
	result = readFrom(PC, true);

	if (opcode == 0xB7 || opcode == 0x97) {
		fmt::format_to(debugBuffer, "{:02X} {:02X}    *{:s} ${:02X},Y @ {:02X} = {:02X}           ",
			opcode, result, currentInstructionName, result, addressAbsolute, readFrom(addressAbsolute, true));
	}
	else {
		fmt::format_to(debugBuffer, "{:02X} {:02X}     {:s} ${:02X},Y @ {:02X} = {:02X}           ",
			opcode, result, currentInstructionName, result, addressAbsolute, readFrom(addressAbsolute, true));
	}
	log();
#endif

	PC++;

	return 0;
}

// Absolute
uint8_t CPU_6502::ABS() {
	uint16_t lo = readFrom(PC);
	uint16_t hi = readFrom(PC+1);
	addressAbsolute = (hi << 8) | lo;

#ifdef _LOG
	if (opcode == 0x0C || opcode == 0xAF || opcode == 0x8F || opcode == 0xCF ||
		opcode == 0xEF || opcode == 0x0F || opcode == 0x2F || opcode == 0x4F ||
		opcode == 0x6F) {
		fmt::format_to(debugBuffer, "{:02X} {:02X} {:02X} *{:s} ${:04X} = {:02X}                ",
			opcode, lo, hi, currentInstructionName, addressAbsolute, readFrom(addressAbsolute, true));
	}
	else if (opcode ==  0x4C || opcode == 0x20) {
		fmt::format_to(debugBuffer, "{:02X} {:02X} {:02X}  {:s} ${:04X}                     ",
			opcode, lo, hi, currentInstructionName, addressAbsolute);
	}
	else {
		fmt::format_to(debugBuffer, "{:02X} {:02X} {:02X}  {:s} ${:04X} = {:02X}                ",
			opcode, lo, hi, currentInstructionName, addressAbsolute, readFrom(addressAbsolute, true));
	}
	log();
#endif

	PC += 2;

	return 0;
}

// Absolute - X Offset
uint8_t CPU_6502::ABX() {
	uint16_t lo = readFrom(PC);
	uint16_t hi = readFrom(PC+1);
	addressAbsolute = (hi << 8) | lo;
	addressAbsolute += X;

#ifdef _LOG
	result = (hi << 8) | lo;

	if (opcode == 0x1C || opcode == 0x3C || opcode == 0x5C || opcode == 0x7C || opcode == 0xDC ||
		opcode == 0xFC || opcode == 0xDF || opcode == 0xFF || opcode == 0x1F || opcode == 0x3F ||
		opcode == 0x5F || opcode == 0x7F) {
		fmt::format_to(debugBuffer, "{:02X} {:02X} {:02X} *{:s} ${:04X},X @ {:04X} = {:02X}       ",
			opcode, lo, hi, currentInstructionName, result,
			addressAbsolute, readFrom(addressAbsolute, true));
	}
	else {
		fmt::format_to(debugBuffer, "{:02X} {:02X} {:02X}  {:s} ${:04X},X @ {:04X} = {:02X}       ",
			opcode, lo, hi, currentInstructionName, result,
			addressAbsolute, readFrom(addressAbsolute, true));
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
	uint16_t lo = readFrom(PC);
	uint16_t hi = readFrom(PC + 1);
	addressAbsolute = (hi << 8) | lo;
	addressAbsolute += Y;

#ifdef _LOG
	result = (hi << 8) | lo;

	if (opcode == 0xBF || opcode == 0xDB || opcode == 0xFB || opcode == 0x1B || opcode == 0x3B ||
		opcode == 0x5B || opcode == 0x7B) {
		fmt::format_to(debugBuffer, "{:02X} {:02X} {:02X} *{:s} ${:04X},Y @ {:04X} = {:02X}       ",
			opcode, lo, hi, currentInstructionName, result, addressAbsolute, readFrom(addressAbsolute, true));
	}
	else {
		fmt::format_to(debugBuffer, "{:02X} {:02X} {:02X}  {:s} ${:04X},Y @ {:04X} = {:02X}       ",
			opcode, lo, hi, currentInstructionName, result, addressAbsolute, readFrom(addressAbsolute, true));
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
	uint16_t ptrLo = readFrom(PC);
	uint16_t ptrHi = readFrom(PC+1);
	uint16_t ptr = (ptrHi << 8) | ptrLo;


	if (ptrLo == 0x00FF) // Simulate page boundary hardware bug
	{
		addressAbsolute = (readFrom(ptrHi << 8) << 8) | readFrom(ptr);
	}
	else // Behave normally
	{
		addressAbsolute = (readFrom(ptr + 1) << 8) | readFrom(ptr);
	}

#ifdef _LOG
	fmt::format_to(debugBuffer, "{:02X} {:02X} {:02X}  {:s} (${:04X}) = {:04X}            ",
		opcode, ptrLo, ptrHi, currentInstructionName, ptr, addressAbsolute);
	log();
#endif

	PC += 2;

	return 0;
}

// Indirect - X Offset
uint8_t CPU_6502::IZX() {
	result = readFrom(PC);
	uint16_t lo = readFrom(lowByte((uint16_t)(result + (uint16_t)X)));
	uint16_t hi = readFrom(lowByte((uint16_t)(result + (uint16_t)X + 1)));

	addressAbsolute = (hi << 8) | lo;

#ifdef _LOG
	if (opcode == 0xA3 || opcode == 0x83 || opcode == 0xC3 || opcode == 0xE3 || opcode == 0x03 ||
		opcode == 0x23 || opcode == 0x43 || opcode == 0x63) {
		fmt::format_to(debugBuffer, "{:02X} {:02X}    *{:s} (${:02X},X) @ {:02X} = {:04X} = {:02X}  ",
			opcode, result, currentInstructionName, result, lowByte(result + X), addressAbsolute, readFrom(addressAbsolute, true));
	}
	else {
		fmt::format_to(debugBuffer, "{:02X} {:02X}     {:s} (${:02X},X) @ {:02X} = {:04X} = {:02X}  ",
			opcode, result, currentInstructionName, result, lowByte(result + X), addressAbsolute, readFrom(addressAbsolute, true));
	}
	log();
#endif

	PC++;

	return 0;
}

// Indirect - Y Offset
uint8_t CPU_6502::IZY() {
	result = readFrom(PC);
	uint16_t lo = readFrom(lowByte(result));
	uint16_t hi = readFrom(lowByte(result + 1));

	addressAbsolute = (hi << 8) | lo;

#ifdef _LOG
	if (opcode == 0xA3 || opcode == 0xB3 || opcode == 0xD3 || opcode == 0xF3 || opcode == 0x13 || opcode == 0x33 ||
		opcode == 0x53 || opcode == 0x73) {
		fmt::format_to(debugBuffer, "{:02X} {:02X}    *{:s} (${:02X}),Y = {:04X} @ {:04X} = {:02X}",
			opcode, result, currentInstructionName, result, addressAbsolute,
			(addressAbsolute + Y) & 0xFFFF, readFrom((addressAbsolute + Y) & 0xFFFF, true));
	}
	else {
		fmt::format_to(debugBuffer, "{:02X} {:02X}     {:s} (${:02X}),Y = {:04X} @ {:04X} = {:02X}",
			opcode, result, currentInstructionName, result, addressAbsolute,
			(addressAbsolute + Y) & 0xFFFF, readFrom((addressAbsolute + Y) & 0xFFFF, true));
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
	addressRelative = readFrom(PC);

	if (addressRelative & 0x80)
		addressRelative |= 0xFF00;

#ifdef _LOG
	fmt::format_to(debugBuffer, "{:02X} {:02X}     {:s} ${:04X}                     ",
		opcode, readFrom(PC, true), currentInstructionName, (PC+1 + addressRelative) & 0xFFFF);
	log();
#endif

	PC++;

	return 0;
}
