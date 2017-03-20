//ePS6800 Mask ROM
#include "rom.hpp"

uint16_t rom[96*1024]; //Max of 96K words

void rom_init() {
    
    /*//LCD Test
    rom[0] = 0x4E00;//A=0x00;
    rom[1] = 0x2122;//r21h = 0x00;
    rom[2] = 0x2123;
    rom[3] = 0x4E44;
    rom[4] = 0x2121;
    rom[5] = 0x4E60;
    rom[6] = 0x270A;
    rom[7] = 0x210E;
    rom[8] = 0x0020;
    rom[9] = 0x0008;*/
    rom[0] = 0x4E60;//A=0X60;
    rom[1] = 0x2113;//R13=0X60;
    rom[2] = 0x4E70;//A=0X70
    rom[3] = 0x1013;//A+=R13;
    rom[4] = 0x0020;//Inf loop
    rom[5] = 0x0004;
}

uint16_t rom_read_word(uint32_t addr) {
    if (addr < 96*1024) {
        return rom[addr];
    }
}

void rom_open(std::string filename) {

}
