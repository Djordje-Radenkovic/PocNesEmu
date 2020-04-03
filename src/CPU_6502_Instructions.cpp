#include <iostream>

#include "CPU_6502.h"

//	+-----------------------+
//	|	  Instructions		|
//	+-----------------------+

// Add with Carry
uint8_t CPU_6502::ADC() {
	fetchData();

	result = (uint16_t)A + (uint16_t)fetchedData + (uint16_t)PS.CF;

	PS.CF = (result > 0xFF);
	checkZF(lowByte(result));
	PS.OF = !(0 == ((~((uint16_t)A ^ (uint16_t)fetchedData) & ((uint16_t)A ^ (uint16_t)result)) & 0x0080));
	checkNF(result);

	A = lowByte(result);

	return 1;
}

// Logical AND
uint8_t CPU_6502::AND() {
	fetchData();

	A &= fetchedData;

	checkZF(A);
	checkNF(A);

	return 1;
}

// Arithmetic Shift Left
uint8_t CPU_6502::ASL() {
	fetchData();

	result = (uint16_t)fetchedData << 1;

	PS.CF = (result & 0xFF00) > 0;
	checkZF(lowByte(result));
	checkNF(lowByte(result));

	if (isIMP())
		A = lowByte(result);
	else
		write(addressAbsolute, lowByte(result));

	return 0;
}

// Branch if Carry Clear
uint8_t CPU_6502::BCC() {
	if (PS.CF == 0) {
		cycles++;
		addressAbsolute = PC + addressRelative;

		if (highByte(addressAbsolute) != highByte(PC))
			cycles++;

		PC = addressAbsolute;
	}

	return 0;
}

// Branch if Carry Set
uint8_t CPU_6502::BCS() {
	if (PS.CF == 1) {
		cycles++;
		addressAbsolute = PC + addressRelative;

		if (highByte(addressAbsolute) != highByte(PC))
			cycles++;

		PC = addressAbsolute;
	}

	return 0;
}

// Branch if Equal
uint8_t CPU_6502::BEQ() {
	if (PS.ZF == 1) {
		cycles++;
		addressAbsolute = PC + addressRelative;

		if (highByte(addressAbsolute) != highByte(PC))
			cycles++;

		PC = addressAbsolute;
	}

	return 0;
}

// Bit Test
uint8_t CPU_6502::BIT() {
	fetchData();

	result = A & fetchedData;

	checkZF(lowByte(result));
	PS.NF = (fetchedData & (1 << 7)) >> 7;
	PS.OF = (fetchedData & (1 << 6)) >> 6;

	return 0;
}

// Branch if Minus
uint8_t CPU_6502::BMI() {
	if (PS.NF == 1) {
		cycles++;
		addressAbsolute = PC + addressRelative;

		if (highByte(addressAbsolute) != highByte(PC))
			cycles++;

		PC = addressAbsolute;
	}

	return 0;
}

// Branch if Not Equal
uint8_t CPU_6502::BNE() {
	if (PS.ZF == 0) {
		cycles++;
		addressAbsolute = PC + addressRelative;

		if (highByte(addressAbsolute) != highByte(PC))
			cycles++;

		PC = addressAbsolute & 0xFFFF;
	}

	return 0;
}

// Branch if Positive
uint8_t CPU_6502::BPL() {
	if (PS.NF == 0) {
		cycles++;
		addressAbsolute = PC + addressRelative;

		if (highByte(addressAbsolute) != highByte(PC))
			cycles++;

		PC = addressAbsolute;
	}

	return 0;
}

// Force Interrupt
uint8_t CPU_6502::BRK() {
	PC++;
	PS.ID = 1;

	push(highByte(PC));
	push(lowByte(PC));

	PS.BC = 1;

	push(PS.data);

	PS.BC = 0;

	PC = (uint16_t)read(irqVectorLow) | ((uint16_t)read(irqVectorHigh) << 8);

	return 0;
}

// Branch if Overflow Clear
uint8_t CPU_6502::BVC() {
	if (PS.OF == 0) {
		cycles++;
		addressAbsolute = PC + addressRelative;

		if (highByte(addressAbsolute) != highByte(PC))
			cycles++;

		PC = addressAbsolute;
	}

	return 0;
}


// Branch if Overflow Set
uint8_t CPU_6502::BVS() {
	if (PS.OF == 1) {
		cycles++;
		addressAbsolute = PC + addressRelative;

		if (highByte(addressAbsolute) != highByte(PC))
			cycles++;

		PC = addressAbsolute;
	}

	return 0;
}

// Clear Carry Flag
uint8_t CPU_6502::CLC() {
	PS.CF = 0;

	return 0;
}

// Clear Decimal Mode
uint8_t CPU_6502::CLD() {
	PS.DM = 0;

	return 0;
}

// Clear Interrupt Disable
uint8_t CPU_6502::CLI() {
	PS.ID = 0;

	return 0;
}

// Clear Overflow Flag
uint8_t CPU_6502::CLV() {
	PS.OF = 0;

	return 0;
}

// Compare
uint8_t CPU_6502::CMP() {
	fetchData();

	result = (uint16_t)A - (uint16_t)fetchedData;

	PS.CF = (A >= fetchedData);
	checkZF(lowByte(result));
	checkNF(lowByte(result));

	return 0;
}

// Compare X Register
uint8_t CPU_6502::CPX() {
	fetchData();

	result = (uint16_t)X - (uint16_t)fetchedData;

	PS.CF = (X >= fetchedData);
	checkZF(lowByte(result));
	checkNF(lowByte(result));

	return 0;
}

// Compare Y Register
uint8_t CPU_6502::CPY() {
	fetchData();

	result = (uint16_t)Y - (uint16_t)fetchedData;

	PS.CF = (Y >= fetchedData);
	checkZF(lowByte(result));
	checkNF(lowByte(result));

	return 0;
}

// Decrement Memory
uint8_t CPU_6502::DEC() {
	result = lowByte(read(addressAbsolute) - 1);

	write(addressAbsolute, result);

	checkZF(result);
	checkNF(result);

	return 0;
}

// Decrement X Register
uint8_t CPU_6502::DEX() {
	X--;

	checkZF(X);
	checkNF(X);

	return 0;
}

// Decrement Y Register
uint8_t CPU_6502::DEY() {
	Y--;

	checkZF(Y);
	checkNF(Y);

	return 0;
}

// Exclusive OR
uint8_t CPU_6502::EOR() {
	fetchData();

	A ^= fetchedData;

	checkZF(A);
	checkNF(A);

	return 1;
}

// Increment Memory
uint8_t CPU_6502::INC() {
	if (isIMP()) {
		A++;

		checkZF(A);
		checkNF(A);
	}
	else {
		result = lowByte(read(addressAbsolute) + 1);

		write(addressAbsolute, result);

		checkZF(result);
		checkNF(result);
	}

	return 0;
}

// Increment X Register
uint8_t CPU_6502::INX() {
	X++;

	checkZF(X);
	checkNF(X);

	return 0;
}

// Increment Y Register
uint8_t CPU_6502::INY() {
	Y++;

	checkZF(Y);
	checkNF(Y);

	return 0;
}

// Jump
uint8_t CPU_6502::JMP() {
	PC = addressAbsolute;

	return 0;
}

// Jump to Subroutine
uint8_t CPU_6502::JSR() {
	result = PC - 1;

	push(highByte(result));
	push(lowByte(result));

	PC = addressAbsolute;

	return 0;
}

// Load Accumulator
uint8_t CPU_6502::LDA() {
	fetchData();

	A = fetchedData;

	checkZF(A);
	checkNF(A);

	return 1;
}

// Load X Register
uint8_t CPU_6502::LDX() {
	fetchData();

	X = fetchedData;

	checkZF(X);
	checkNF(X);

	return 1;
}

// Load Y Register
uint8_t CPU_6502::LDY() {
	fetchData();

	Y = fetchedData;

	checkZF(Y);
	checkNF(Y);

	return 1;
}

// Logical Shift Right
uint8_t CPU_6502::LSR() {
	fetchData();

	PS.CF = fetchedData & 0x0001;

	result = fetchedData >> 1;

	checkZF(lowByte(result));
	checkNF(lowByte(result));

	if (isIMP())
		A = lowByte(result);
	else
		write(addressAbsolute, lowByte(result));

	return 0;
}

// No Operation
uint8_t CPU_6502::NOP() {
	// Multiple NOPs, check which one
	// https://wiki.nesdev.com/w/index.php/CPU_unofficial_opcodes

	switch (opcode) {
	case 0x04:
	case 0x1C:
	case 0x3C:
	case 0x5C:
	case 0x7C:
	case 0xDC:
	case 0xFC:
		return 1;
		break;
	}

	return 0;
}

// Logical Inclusive OR
uint8_t CPU_6502::ORA() {
	fetchData();

	A |= fetchedData;

	checkZF(A);
	checkNF(A);

	return 1;
}

// Push Accumulator
uint8_t CPU_6502::PHA() {
	push(A);

	return 0;
}

// Push Processor Status
uint8_t CPU_6502::PHP() {
	PS.BC = 1;
	PS.XX = 1;
	push(PS.data);

	PS.BC = 0;
	PS.XX = 0;

	return 0;
}

// Pull Accumulator
uint8_t CPU_6502::PLA() {
	A = pull();

	checkZF(A);
	checkNF(A);

	return 0;
}

// Pull Processor Status
uint8_t CPU_6502::PLP() {
	PS.data = pull();

	PS.XX = 1;
	PS.BC = 0;	// TODO: Verify if this should be here (kind of a hack)


	return 0;
}

// Rotate Left
uint8_t CPU_6502::ROL() {
	fetchData();

	result = (uint16_t)(fetchedData << 1) | PS.CF;

	checkCF(result);
	checkZF(lowByte(result));
	checkNF(lowByte(result));

	if (isIMP())
		A = lowByte(result);
	else
		write(addressAbsolute, lowByte(result));

	return 0;
}

// Rotate Right
uint8_t CPU_6502::ROR() {
	fetchData();

	result = (uint16_t)(PS.CF << 7) | (fetchedData >> 1);

	PS.CF = fetchedData & 0x01;
	checkZF(lowByte(result));
	checkNF(lowByte(result));

	if (isIMP())
		A = lowByte(result);
	else
		write(addressAbsolute, lowByte(result));

	return 0;
}

// Return from Interrupt
uint8_t CPU_6502::RTI() {
	PS.data = pull();
	// PS.data &= ~PS.BC
	// PS.data &= ~PS.XX

	PC = (uint16_t)pull();
	PC |= (uint16_t)pull() << 8;

	return 0;
}

// Return from Subroutine
uint8_t CPU_6502::RTS() {
	PC = (uint16_t)pull();
	PC |= (uint16_t)pull() << 8;

	PC++;

	return 0;
}

// Subtract with Carry
uint8_t CPU_6502::SBC() {
	fetchData();

	result = (uint16_t)A + ((uint16_t)fetchedData ^ 0x00FF) + (uint16_t)PS.CF;

	PS.CF = (result > 0xFF);
	checkZF(lowByte(result));
	PS.OF = !(0 == ((result ^ (uint16_t)A) & (result ^ ((uint16_t)fetchedData ^ 0x00FF)) & 0x0080));
	checkNF(lowByte(result));

	A = lowByte(result);

	return 1;
}

// Set Carry Flag
uint8_t CPU_6502::SEC() {
	PS.CF = 1;

	return 0;
}

// Set Decimal Flag
uint8_t CPU_6502::SED() {
	PS.DM = 1;

	return 0;
}

// Set Interrupt Disable
uint8_t CPU_6502::SEI() {
	PS.ID = 1;

	return 0;
}

// Store Accumulator
uint8_t CPU_6502::STA() {
	write(addressAbsolute, A);

	return 0;
}

// Store X Register
uint8_t CPU_6502::STX() {
	write(addressAbsolute, X);

	return 0;
}

// Store Y Register
uint8_t CPU_6502::STY() {
	write(addressAbsolute, Y);

	return 0;
}

// Transfer Accumulator to X
uint8_t CPU_6502::TAX() {
	X = A;

	checkZF(X);
	checkNF(X);

	return 0;
}

// Transfer Accumulator to Y
uint8_t CPU_6502::TAY() {
	Y = A;

	checkZF(Y);
	checkNF(Y);

	return 0;
}

// Transfer Stack Pointer to X
uint8_t CPU_6502::TSX() {
	X = SP;

	checkZF(X);
	checkNF(X);

	return 0;
}

// Transfer X to Accumulator
uint8_t CPU_6502::TXA() {
	A = X;

	checkZF(A);
	checkNF(A);

	return 0;
}


// Transfer A to Stack Pointer
uint8_t CPU_6502::TXS() {
	SP = X;

	return 0;
}

// Transfer Y to Accumulator
uint8_t CPU_6502::TYA() {
	A = Y;

	checkZF(A);
	checkNF(A);

	return 0;
}


//	+-------------------------------+
//	|	  Illegal Instructions		|
//	+-------------------------------+


// Load Acumulator and X Register
uint8_t CPU_6502::LAX() {
	fetchData();

	A = fetchedData;
	X = fetchedData;

	checkZF(A);
	checkNF(A);

	return 1;
}

// Store Acumulator AND X Register
uint8_t CPU_6502::SAX() {
	write(addressAbsolute, A & X);

	return 0;
}

// Decrement Memory and Acumulator
uint8_t CPU_6502::DCP() {
	result = lowByte(read(addressAbsolute) - 1);

	write(addressAbsolute, result);

	result = (uint16_t)A - (uint16_t)result;

	PS.CF = (A >= result);
	checkZF(lowByte(result));
	checkNF(lowByte(result));

	return 0;
}

// Unknown instructions
uint8_t CPU_6502::XXX() {
	if (opcode == 0x32) {
		fmt::print(" JAM instruction");
		std::cin.get();

		return 0;
	}
	
	fmt::print(" Unknown instruction");

	return 0;
}
