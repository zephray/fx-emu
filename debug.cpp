//ePS6800 Debug Related Functions

#include "debug.hpp"

char *reg_names[] = {
	"INDF0",
	"FSR0",
	"BSR",
	"INDF1",
	"FSR1",
	"BSR1",
	"STKPTR",
	"PCL",
	"PCM",
	"PCH",
	"ACC",
	"TABPTRL",
	"TABPTRM",
	"TABPTRH",
	"LCDDATA",
	"STATUS",
	"INDF2",
	"FSR2",
	"BSR2",
	"General RAM",
	"General RAM",
	"General RAM",
	"General RAM",
	"General RAM",
	"General RAM",
	"General RAM",
	"General RAM",
	"General RAM",
	"General RAM",
	"General RAM",
	"General RAM",
	"General RAM",
	"CPUCON",
	"POST_ID",
	"LCDARL",
	"LCDARH",
	"INTSTA",
	"TR0CON",
	"TRL0L",
	"TRL0H",
	"T0CL",
	"T0CH",
	"TR1CON",
	"TRL1",
	"TR2WCON",
	"TRL2",
	"LCDCON",
	"R2F",
	"STBCON",
	"PORTA",
	"PACON",
	"DCRA",
	"PAWAKE",
	"PAINTEN",
	"PAINTSTA",
	"PORTB",
	"PBCON",
	"DCRB",
	"PORTC",
	"PCCON",
	"DCRC",
	"PORTD",
	"PORTE",
	"DCRDE"
};

struct Instruction {
	uint16_t instrMask;
	uint16_t instrPattern;
	uint16_t instrWordNum;
	uint32_t instrImmNum;
	uint32_t instrImm1Mask;
	uint32_t instrImm2Mask;
	uint32_t instrImm3Mask;
	char * instrText;
};

const int INSTR_COUNT = 85;

const Instruction instructionSet[INSTR_COUNT + 1] = { //Include one unknown instruction
	{ 0xFFFF, 0x0000, 1, 0, 0, 0, 0, "NOP" },
	{ 0xFFFF, 0x0001, 1, 0, 0, 0, 0, "WDTC" },
	{ 0xFFFF, 0x0002, 1, 0, 0, 0, 0, "SLEP" },
	{ 0xFF00, 0x2700, 1, 1, 0x00FF, 0, 0, "RPT r%02xh" },
	{ 0xFF00, 0x4300, 1, 1, 0x00FF, 0, 0, "BANK #%02xh" },
	{ 0xF000, 0x3000, 1, 1, 0x0FFF, 0, 0, "S0CALL %03xh" },
	{ 0xE000, 0xE000, 1, 1, 0x1FFF, 0, 0, "SCALL %04xh" },
	{ 0xFFFE, 0x0030, 2, 1, 0x0001FFFF, 0, 0, "LCALL %05xh" },
	{ 0xFFFF, 0x2BFE, 1, 0, 0, 0, 0, "RET" },
	{ 0xFFFF, 0x2BFF, 1, 0, 0, 0, 0, "RETI" },
	{ 0xFF00, 0x2500, 1, 1, 0x00FF, 0, 0, "TEST r%02xh" },
	{ 0xE000, 0xC000, 1, 1, 0x1FFF, 0, 0, "SJMP %04xh" },
	{ 0xFFFE, 0x0020, 2, 1, 0x0001FFFF, 0, 0, "LJMP %05xh" },
	{ 0xFF00, 0x5000, 2, 2, 0x00FF0000, 0x0000FFFF, 0, "JDNZ A, r%02xh, %04xh" },
	{ 0xFF00, 0x5100, 2, 2, 0x00FF0000, 0x0000FFFF, 0, "JDNZ r%02xh, %04xh" },
	{ 0xFF00, 0x4700, 2, 2, 0x00FF0000, 0x0000FFFF, 0, "JGE A, #%02xh, %04xh" },
	{ 0xFF00, 0x4800, 2, 2, 0x00FF0000, 0x0000FFFF, 0, "JLE A, #%02xh, %04xh" },
	{ 0xFF00, 0x4900, 2, 2, 0x00FF0000, 0x0000FFFF, 0, "JE A, #%02xh, %04xh" },
	{ 0xFF00, 0x5500, 2, 2, 0x00FF0000, 0x0000FFFF, 0, "JGE A, r%02xh, %04xh" },
	{ 0xFF00, 0x5600, 2, 2, 0x00FF0000, 0x0000FFFF, 0, "JLE A, r%02xh, %04xh" },
	{ 0xFF00, 0x5700, 2, 2, 0x00FF0000, 0x0000FFFF, 0, "JE A, r%02xh, %04xh" },
	{ 0xF800, 0x5800, 2, 3, 0x00FF0000, 0x07000000, 0x0000FFFF, "JBC r%02xh, %d, %04xh" },
	{ 0xF800, 0x6000, 2, 3, 0x00FF0000, 0x07000000, 0x0000FFFF, "JBS r%02xh, %d, %04xh" },
	{ 0xFF00, 0x2000, 1, 1, 0x00FF, 0, 0, "MOV A, r%02xh" },
	{ 0xFF00, 0x2100, 1, 1, 0x00FF, 0, 0, "MOV r%02xh, A" },
	{ 0xE000, 0x8000, 1, 2, 0x1F00, 0x00FF, 0, "MOVRP p%02xh, r%02xh" },
	{ 0xE000, 0xA000, 1, 2, 0x00FF, 0x1F00, 0, "MOVPR r%02xh, p%02xh" },
	{ 0xFF00, 0x4E00, 1, 1, 0x00FF, 0, 0, "MOV A, #%02xh" },
	{ 0xFF00, 0x2400, 1, 1, 0x00FF, 0, 0, "CLR r%02xh" },
	{ 0xFF00, 0x4000, 1, 1, 0x00FF, 0, 0, "TBPTL #%02xh" },
	{ 0xFF00, 0x4100, 1, 1, 0x00FF, 0, 0, "TBPTM #%02xh" },
	{ 0xFF00, 0x4200, 1, 1, 0x00FF, 0, 0, "TBPTH #%02xh" },
	{ 0xFF00, 0x2F00, 1, 1, 0x00FF, 0, 0, "TBRD A, r%02xh" },//Special case of TBRD
	{ 0xFC00, 0x2C00, 1, 2, 0x0300, 0x00FF, 0, "TBRD, %d, r%02xh" },//General case
	{ 0xFF00, 0x0200, 1, 1, 0x00FF, 0, 0, "OR A, r%02xh" },
	{ 0xFF00, 0x0300, 1, 1, 0x00FF, 0, 0, "OR r%02xh, A" },
	{ 0xFF00, 0x4400, 1, 1, 0x00FF, 0, 0, "OR A, #%02xh" },
	{ 0xFF00, 0x0400, 1, 1, 0x00FF, 0, 0, "AND A, r%02xh" },
	{ 0xFF00, 0x0500, 1, 1, 0x00FF, 0, 0, "AND r%02xh, A" },
	{ 0xFF00, 0x4500, 1, 1, 0x00FF, 0, 0, "AND A, #%02xh" },
	{ 0xFF00, 0x0600, 1, 1, 0x00FF, 0, 0, "XOR A, r%02xh" },
	{ 0xFF00, 0x0700, 1, 1, 0x00FF, 0, 0, "XOR r%02xh, A" },
	{ 0xFF00, 0x4600, 1, 1, 0x00FF, 0, 0, "XOR A, #%02xh" },
	{ 0xFF00, 0x0800, 1, 1, 0x00FF, 0, 0, "COMA r%02xh" },
	{ 0xFF00, 0x0900, 1, 1, 0x00FF, 0, 0, "COM r%02xh" },
	{ 0xFF00, 0x1C00, 1, 1, 0x00FF, 0, 0, "INCA r%02xh" },
	{ 0xFF00, 0x1D00, 1, 1, 0x00FF, 0, 0, "INC r%02xh" },
	{ 0xFF00, 0x1000, 1, 1, 0x00FF, 0, 0, "ADD A, r%02xh" },
	{ 0xFF00, 0x1100, 1, 1, 0x00FF, 0, 0, "ADD r%02xh, A" },
	{ 0xFF00, 0x4A00, 1, 1, 0x00FF, 0, 0, "ADD A, #%02xh" },
	{ 0xFF00, 0x1200, 1, 1, 0x00FF, 0, 0, "ADC A, r%02xh" },
	{ 0xFF00, 0x1300, 1, 1, 0x00FF, 0, 0, "ADC r%02xh, A" },
	{ 0xFF00, 0x4B00, 1, 1, 0x00FF, 0, 0, "ADC A, #%02xh" },
	{ 0xFF00, 0x1E00, 1, 1, 0x00FF, 0, 0, "DECA r%02xh" },
	{ 0xFF00, 0x1F00, 1, 1, 0x00FF, 0, 0, "DEC r%02xh" },
	{ 0xFF00, 0x1600, 1, 1, 0x00FF, 0, 0, "SUB A, r%02xh" },
	{ 0xFF00, 0x1700, 1, 1, 0x00FF, 0, 0, "SUB r%02xh, A" },
	{ 0xFF00, 0x4C00, 1, 1, 0x00FF, 0, 0, "SUB A, #%02xh" },
	{ 0xFF00, 0x1800, 1, 1, 0x00FF, 0, 0, "SUBB A, r%02xh" },
	{ 0xFF00, 0x1900, 1, 1, 0x00FF, 0, 0, "SUBB r%02xh, A" },
	{ 0xFF00, 0x4D00, 1, 1, 0x00FF, 0, 0, "SUBB A, #%02xh" },
	{ 0xFF00, 0x1400, 1, 1, 0x00FF, 0, 0, "ADDDC A, r%02xh" },
	{ 0xFF00, 0x1500, 1, 1, 0x00FF, 0, 0, "ADDDC r%02xh, A" },
	{ 0xFF00, 0x1A00, 1, 1, 0x00FF, 0, 0, "SUBDB A, r%02xh" },
	{ 0xFF00, 0x1B00, 1, 1, 0x00FF, 0, 0, "SUBDB r%02xh, A" },
	{ 0xFF00, 0x0A00, 1, 1, 0x00FF, 0, 0, "RRCA r%02xh" },
	{ 0xFF00, 0x0B00, 1, 1, 0x00FF, 0, 0, "RRC r%02xh" },
	{ 0xFF00, 0x0C00, 1, 1, 0x00FF, 0, 0, "RLCA r%02xh" },
	{ 0xFF00, 0x0D00, 1, 1, 0x00FF, 0, 0, "RLC r%02xh" },
	{ 0xFF00, 0x2200, 1, 1, 0x00FF, 0, 0, "SHRA r%02xh" },
	{ 0xFF00, 0x2300, 1, 1, 0x00FF, 0, 0, "SHLA r%02xh" },
	{ 0xFF00, 0x5400, 1, 1, 0x00FF, 0, 0, "EX r%02xh" },
	{ 0xF800, 0x6800, 1, 2, 0x00FF, 0x0700, 0, "BC r%02xh, %d" },
	{ 0xF800, 0x7000, 1, 2, 0x00FF, 0x0700, 0, "BS r%02xh, %d" },
	{ 0xF800, 0x7800, 1, 2, 0x00FF, 0x0700, 0, "BTG r%02xh, %d" },
	{ 0xFF00, 0x5200, 1, 1, 0x00FF, 0, 0, "EXL r%02xh" },
	{ 0xFF00, 0x5300, 1, 1, 0x00FF, 0, 0, "EXH r%02xh" },
	{ 0xFF00, 0x2600, 1, 1, 0x00FF, 0, 0, "MOVL r%02xh, A" },
	{ 0xFF00, 0x2800, 1, 1, 0x00FF, 0, 0, "MOVH r%02xh, A" },
	{ 0xFF00, 0x2900, 1, 1, 0x00FF, 0, 0, "MOVL A, r%02xh" },
	{ 0xFF00, 0x2A00, 1, 1, 0x00FF, 0, 0, "MOVH A, r%02xh" },
	{ 0xFF00, 0x0100, 1, 1, 0x00FF, 0, 0, "SFR4 r%02xh" },
	{ 0xFF00, 0x4F00, 1, 1, 0x00FF, 0, 0, "SFL4 r%02xh" },
	{ 0xFF00, 0x0F00, 1, 1, 0x00FF, 0, 0, "SWAP r%02xh" },
	{ 0xFF00, 0x0E00, 1, 1, 0x00FF, 0, 0, "SWAPA r%02xh" },
	{ 0x0000, 0x0000, 1, 0, 0, 0, 0, "???" }
};

int firstOneFromRight(uint32_t data) {
	for (int i = 0; i < 32; i++) {
		if ((data >> i) & 0x01)
			return i;
	}
	return 0;//should be -1, but here if it is not find, we do not care the value
}

char *disAsm(uint16_t addr, uint32_t instr) {
	char baseout[20];
	char instrout[80];
	char *result;
	int size;
	int i;
	bool matchFound;
	uint32_t imm1, imm2, imm3;
	uint32_t address;
	uint32_t instruction;
	
	matchFound = false;
	address = addr;
	instruction = instr >> 16;
	for (i = 0; ((i < INSTR_COUNT + 1) && (!matchFound)); i++) {
		if ((instruction & instructionSet[i].instrMask) == instructionSet[i].instrPattern) {
			matchFound = true;
			if (instructionSet[i].instrWordNum == 2) {
				instruction = instr;
				snprintf(baseout, sizeof(baseout), "%04x: %08x  ", address, instruction);
			}
			else {
				snprintf(baseout, sizeof(baseout), "%04x: %04x      ", address, (uint16_t)instruction);
			}
			imm1 = instruction & instructionSet[i].instrImm1Mask;
			imm2 = instruction & instructionSet[i].instrImm2Mask;
			imm3 = instruction & instructionSet[i].instrImm3Mask;
			imm1 >>= firstOneFromRight(instructionSet[i].instrImm1Mask);
			imm2 >>= firstOneFromRight(instructionSet[i].instrImm2Mask);
			imm3 >>= firstOneFromRight(instructionSet[i].instrImm3Mask);
			snprintf(instrout, sizeof(instrout), instructionSet[i].instrText, imm1, imm2, imm3);
		}
	}
	size = strlen(baseout) + strlen(instrout) + 2;
	result = (char *)malloc(size);
	snprintf(result, size, "%s%s", baseout, instrout);

	return result;
}
