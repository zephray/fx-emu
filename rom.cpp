//ePS6800 Mask ROM
#include "rom.hpp"

uint8_t rom[96*1024*2]; //Max of 96K words

void rom_init() {
    /*
    //LCD Test
	rom[0] = 0x4E; rom[1] = 0x00;//A=0x00;
    rom[2] = 0x21; rom[3] = 0x22;//r21h = 0x00;
    rom[4] = 0x21; rom[5] = 0x23;
    rom[6] = 0x4E; rom[7] = 0x44;
    rom[8] = 0x21; rom[9] = 0x21;
    rom[10] = 0x4E; rom[11] = 0x01;
    rom[12] = 0x27; rom[13] = 0x0A;
    rom[14] = 0x21; rom[15] = 0x0E;
    rom[16] = 0x00; rom[17] = 0x20;
    rom[18] = 0x00; rom[19] = 0x08;*/
    /*//ALU Test
	rom[0] = 0x4E60;//A=0X60;
    rom[1] = 0x2113;//R13=0X60;
    rom[2] = 0x4E70;//A=0X70
    rom[3] = 0x1013;//A+=R13;
    rom[4] = 0x0020;//Inf loop
    rom[5] = 0x0004;*/
	//ALU Test 2 SUB
	rom[0] = 0x4E; rom[1] = 0x50;//A=0x50;
	rom[2] = 0x4C; rom[3] = 0x90;//SUB A, #90
	rom[4] = 0x00; rom[5] = 0x20;
	rom[6] = 0x00; rom[7] = 0x02;
}

uint16_t rom_read_word(uint32_t addr) {
    if (addr < 96*1024) {
        return rom[addr*2] << 8 | rom[addr*2+1];
	}
	else {
		return 0;
	}
}

void rom_open(const char * filename) {
	FILE * pFile;
	long lSize;

	fopen_s(&pFile, filename, "rb");
	if (pFile != NULL) {
		fseek(pFile, 0, SEEK_END);
		lSize = ftell(pFile);
		rewind(pFile);
		fread(rom, 1, lSize, pFile);
		fclose(pFile);
	}
}
