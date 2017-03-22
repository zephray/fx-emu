//ePS6800 Register & Memory

#include "mmio.hpp"
#include "lcd.hpp"

using namespace std;

uint8_t static regs[0x7f];
uint8_t static ram_wbk[27]; //Mapped into 0x25-0x3F if WBK=1
uint8_t static ram[64*128];

void mmio_bad_read_byte(uint8_t addr) {
    
}

void mmio_bad_write_byte(uint8_t addr) {

}

//Address:
//00-FF: Regular Register and Memory Bank 0
uint8_t mmio_read_byte(uint8_t addr) {
    uint8_t byte;
    if (addr&0x80) {
        return ram[addr&0x7F];
    } else {
        switch (addr) {
            case REG_POSTID:
            case REG_LCDARH:
            case REG_LCDARL:
            case REG_LCDDAT:
            case REG_LCDCON: byte = lcd_read_byte(addr);
                             break;
            default: byte = regs[addr]; break;
        }
        return byte;
    }
}

void mmio_write_byte_internal(uint8_t addr, uint8_t byte) {
    regs[addr] = byte;
}

uint8_t mmio_read_byte_internal(uint8_t addr) {
    return regs[addr];
}

void mmio_write_byte(uint8_t addr, uint8_t byte) {
    //printf("Writing byte %02x at addr %02x\n", byte, addr);
    if (addr&0x80) {
        ram[addr&0x7F] = byte;
    } else {
        regs[addr] = byte;
        switch (addr) {
            case REG_POSTID:
            case REG_LCDARH:
            case REG_LCDARL:
            case REG_LCDDAT:
            case REG_LCDCON: lcd_write_byte(addr, byte);
                             break;
        }
    }
}

//Address:
//0000-00FF: Regular memory one would expect
//0100-017F: RAM Bank 0
//0180-3FFF: Other RAM Banks
uint8_t mmio_mem_read_byte(uint16_t addr) {
	return 0;//fix me
}

void mmio_mem_write_byte(uint16_t addr, uint8_t byte) {

}

void mmio_init() {
    int i;
    mmio_reset();
    for (i = 0; i < 27; i++) ram_wbk[i] = 0x00;
    //we don't care about banked memory
}

void mmio_reset() {
    int i;
    for (i = 0; i < 0x12; i++) regs[i] = 0x00;
    for (i = 0x20; i < 0x3C; i++) regs[i] = 0x00;//only clear regs
    regs[0x04] = 0x80;
    regs[0x11] = 0x80;
    regs[0x21] = 0x70;
    regs[0x33] = 0xFF;
    regs[0x39] = 0xFF;
    regs[0x3C] = 0x0F;
}

void *mmio_save_state(size_t *size) {
	return NULL;
}

void mmio_load_state(void *state) {

}
