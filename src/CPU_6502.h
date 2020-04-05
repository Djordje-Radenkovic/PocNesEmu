#pragma once

#include <vector>
#include <map>
#include <fstream>

#include "fmt/format.h"

#include "INesCpu.h"
#include "IBusMaster.h"


#define stackBase		0x0100
#define irqVectorHigh	0xFFFF
#define irqVectorLow	0xFFFE
#define nmiVectorHigh	0xFFFB
#define nmiVectorLow	0xFFFA

#define pull() (readFrom(stackBase + ++SP))
#define push(value) (writeTo(stackBase + SP--, value))

#define lowByte(value) (uint8_t)(value & 0x00FF)
#define highByte(value) (uint8_t)((value >> 8) & 0x00FF)

#define checkZF(value) (PS.ZF = (value == 0x00))
#define checkNF(value) (PS.NF = ((value & 0x80) >> 7))
#define checkCF(value) (PS.CF = ((value & 0x100) >> 8))


class CPU_6502 final : public INesCpu {
private:
	//			+--------------------+
	//			|   CPU Registers	 |
	//			+--------------------+
	uint8_t A = 0x00;		//		 Accumulator
	uint8_t X = 0x00;		//  Index Register X
	uint8_t Y = 0x00;		//  Index Register Y

	uint8_t SP = 0xFD;	//     Stack Pointer
	uint16_t PC = 0x0000;	//   Program Counter

	union PocessorStatus {
		struct {
			uint8_t CF : 1;	//		  Carry Flag
			uint8_t ZF : 1;	//		   Zero Flag
			uint8_t ID : 1;	// Interrupt Disable
			uint8_t DM : 1; //		Decimal Mode
			uint8_t BC : 1; //	   Break Command
			uint8_t XX : 1; //	  	 ------      
			uint8_t OF : 1; //	   Overflow Flag
			uint8_t NF : 1; //	   Negative Flag
		};

		uint8_t data;
	} PS;				    //	Processor Status


	//		  +------------------------+
	//		  |    Addressing Modes    |
	//		  +------------------------+
	uint8_t IMP();	uint8_t IMM(); uint8_t ZP0();
	uint8_t ZPX();	uint8_t ZPY(); uint8_t REL();
	uint8_t ABS();	uint8_t ABX(); uint8_t ABY();
	uint8_t IND();	uint8_t IZX(); uint8_t IZY();


	//			+--------------------+
	//			|	   OP Codes		 |
	//			+--------------------+
	uint8_t ADC();	uint8_t AND();	uint8_t ASL();	uint8_t BCC();
	uint8_t BCS();	uint8_t BEQ();	uint8_t BIT();	uint8_t BMI();
	uint8_t BNE();	uint8_t BPL();	uint8_t BRK();	uint8_t BVC();
	uint8_t BVS();	uint8_t CLC();	uint8_t CLD();	uint8_t CLI();
	uint8_t CLV();	uint8_t CMP();	uint8_t CPX();	uint8_t CPY();
	uint8_t DEC();	uint8_t DEX();	uint8_t DEY();	uint8_t EOR();
	uint8_t INC();	uint8_t INX();	uint8_t INY();	uint8_t JMP();
	uint8_t JSR();	uint8_t LDA();	uint8_t LDX();	uint8_t LDY();
	uint8_t LSR();	uint8_t NOP();	uint8_t ORA();	uint8_t PHA();
	uint8_t PHP();	uint8_t PLA();	uint8_t PLP();	uint8_t ROL();
	uint8_t ROR();	uint8_t RTI();	uint8_t RTS();	uint8_t SBC();
	uint8_t SEC();	uint8_t SED();	uint8_t SEI();	uint8_t STA();
	uint8_t STX();	uint8_t STY();	uint8_t TAX();	uint8_t TAY();
	uint8_t TSX();	uint8_t TXA();	uint8_t TXS();	uint8_t TYA();

	// Illegal opcodes
	uint8_t LAX();	uint8_t SAX();	uint8_t DCP();	uint8_t ISB();
	uint8_t SLO();	uint8_t RLA();	uint8_t SRE();	uint8_t RRA();

	uint8_t XXX(); // Unknown opcodes

	//			+--------------------+
	//			|  Bus Functionality |
	//			+--------------------+
	inline uint8_t readFrom(uint16_t address) override {
		return m_bus->read(address);
	}

	inline void writeTo(uint16_t address, uint8_t data) override {
		m_bus->write(address, data);
	}

	//			+--------------------+
	//			|		Other		 |
	//			+--------------------+

	struct CpuInstruction;

	char currentInstructionName[4];
	fmt::memory_buffer debugBuffer;
	void log();
	bool isIMP();
	bool isIMM();

	uint8_t fetchData();
	uint8_t fetchedData = 0x00;

	uint16_t result = 0x0000;
	uint16_t addressAbsolute = 0x0000;
	uint16_t addressRelative = 0x0000;
	uint8_t opcode = 0x00;
	uint8_t cycles = 0;
	size_t totalCyclesPassed = 0;
	std::ofstream m_debugFile;

	static std::vector<CpuInstruction> lookup;

public:
	std::map<uint16_t, std::string> disassemble(uint16_t nStart, uint16_t nStop);

	CPU_6502();
	CPU_6502(const char * debugFilePath);

	~CPU_6502();

	void connectBus(IBus<uint16_t, uint8_t>* bus) override;
	bool isFinished() override;
	inline size_t getCyclesPassed() override { return totalCyclesPassed; }

	void reset() override;
	void irq()	 override;
	void nmi()	 override;
	void tick()	 override;
};
