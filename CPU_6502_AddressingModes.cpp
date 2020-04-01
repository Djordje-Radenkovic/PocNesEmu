#include "CPU_6502.h"

#define _DEBUG_LOG

void CPU_6502::log() {
	printf_s("\n%04X  %s   A:%02X X:%02X Y:%02X P:%02X SP:%02X PPU:%3d,%3d CYC:%d",
		PC-1, debugBuffer, A, X, Y, PS.data, SP, 0, 0, totalCyclesPassed);
	
	if (m_debugFile != nullptr) {
		fprintf_s(m_debugFile, "\n%04X  %s    A:%02X X:%02X Y:%02X P:%02X SP:%02X PPU:%3d,%3d CYC:%d",
			PC - 1, debugBuffer, A, X, Y, PS.data, SP, 0, 0, totalCyclesPassed);
	}
}

//	+-----------------------+
//	|	Addressing Modes	|
//	+-----------------------+

// Implied
uint8_t CPU_6502::IMP() {
	fetchedData = A;

#ifdef _DEBUG_LOG
	sprintf_s(debugBuffer, "%02X        %s                          ",
		opcode, currentInstructionName);
	log();
#endif

	return 0;
}

// Immediate
uint8_t CPU_6502::IMM() {
	addressAbsolute = read(PC);

#ifdef _DEBUG_LOG
	sprintf_s(debugBuffer, "%02X %02X     %s #$%02X                     ",
		opcode, addressAbsolute, currentInstructionName, addressAbsolute);
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
	sprintf_s(debugBuffer, "%02X %02X     %s $%02X = %02X                 ",
		opcode, addressAbsolute, currentInstructionName, addressAbsolute, read(addressAbsolute));
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
	sprintf_s(debugBuffer, "%02X %02X     *%s $%02X,X @ %02X =  %02X       ",
		opcode, result, currentInstructionName, result, addressAbsolute, read(addressAbsolute));
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
	sprintf_s(debugBuffer, "%02X %02X     *%s $%02X,Y @ %02X =  %02X       ",
		opcode, result, currentInstructionName, result, addressAbsolute, read(addressAbsolute));
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
	sprintf_s(debugBuffer, "%02X %02X %02X  %s $%04X                    ",
		opcode, lo, hi, currentInstructionName, addressAbsolute);
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
	sprintf_s(debugBuffer, "%02X %02X %02X  %s $%04X,X @ %04X = %02X      ",
		opcode, lo, hi, currentInstructionName, result,
		addressAbsolute, read(addressAbsolute));
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
	sprintf_s(debugBuffer, "%02X %02X %02X  %s $%04X,Y @ %04X = %02X      ",
		opcode, lo, hi, currentInstructionName, result,
		addressAbsolute, read(addressAbsolute));
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

	addressAbsolute = (read(ptr + 1) << 8) | read(ptr);

#ifdef _DEBUG_LOG
	// result = read(PC);
	// addressAbsolute = read(PC + 1);
	// addressRelative = (addressAbsolute << 8) | result;
	sprintf_s(debugBuffer, "%02X %02X %02X  %s ($%04X) = %04X           ",
		opcode, ptrLo, ptrHi, currentInstructionName, addressAbsolute, read(addressAbsolute));
	log();
#endif

	PC += 2;

	return 0;
}

// Indirect - X Offset
uint8_t CPU_6502::IZX() {
	uint16_t temp = read(PC++);
	uint16_t lo = read((uint16_t)(temp + (uint16_t)X) & 0x00FF);
	uint16_t hi = read((uint16_t)(temp + (uint16_t)X + 1) & 0x00FF);

	addressAbsolute = (hi << 8) | lo;

	return 0;
}

// Indirect - Y Offset
uint8_t CPU_6502::IZY() {
	uint16_t temp = read(PC++);
	uint16_t lo = read(temp & 0x00FF);
	uint16_t hi = read((temp + 1) & 0x00FF);

	addressAbsolute = (hi << 8) | lo;
	addressAbsolute += Y;

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
	// result = read(PC);
	// addressAbsolute = read(PC + 1);
	// addressRelative = addressAbsolute;
	sprintf_s(debugBuffer, "%02X %02X     %s $%04X                    ",
		opcode, read(PC), currentInstructionName, addressRelative);
	log();
#endif

	PC++;

	return 0;
}
