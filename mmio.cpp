//ePS6800 Register & Memory

#include "mmio.hpp"
#include "lcd.hpp"

using namespace std;

uint8_t static regs[0x7f];
uint8_t static ram_wbk[27]; //Mapped into 0x25-0x3F if WBK=1
uint8_t static ram[64*128];

#define mem_page (uint16_t)(regs[REG_BSR]&0x3F)

void mmio_bad_read_byte(uint8_t addr) {
    
}

void mmio_bad_write_byte(uint8_t addr) {

}

//Address:
//00-FF: Regular Register and Memory Bank 0
uint8_t mmio_read_byte(uint8_t addr) {
    uint8_t byte;
    if (addr&0x80) {
        return ram[(mem_page << 7) | addr & 0x7F];
    } else {
		if ((addr >= 0x25) && (addr <= 0x3F) && (regs[REG_CPUCON] & BIT_WBK)) {
			return ram_wbk[addr - 0x25];
		}
		else {
			switch (addr) {
			case REG_INDF0:
				if (regs[REG_FSR0] & 0x80) {
					byte = ram[((regs[REG_BSR] & 0x3F) << 7) | (regs[REG_FSR0] & 0x7F)];
				}
				else {
					if (regs[REG_BSR] != 0) mmio_bad_read_byte(addr);
					byte = regs[REG_FSR0];
				}
				if (regs[REG_POSTID] & BIT_FSR0PE) {
					if (regs[REG_POSTID] & BIT_FSR0ID) { //Auto Inc
						regs[REG_FSR0] ++;
					}
					else { //Auto Dec
						regs[REG_FSR0] --;
					}
				}
				break;
			case REG_INDF1:
				byte = ram[((regs[REG_BSR1] & 0x3F) << 7) | (regs[REG_FSR1] & 0x7F)];
				if (regs[REG_POSTID] & BIT_FSR1PE) {
					if (regs[REG_POSTID] & BIT_FSR1ID) { //Auto Inc
						if (regs[REG_FSR1] == 0xFF)
							regs[REG_BSR1] ++; //carry
						regs[REG_FSR1] ++;
					}
					else { //Auto Dec
						if (regs[REG_FSR1] == 0x00)
							regs[REG_BSR1] --; //borrow
						regs[REG_FSR1] --;
					}
				}
				break;
			case REG_INDF2:
				byte = ram[((regs[REG_BSR2] & 0x3F) << 7) | (regs[REG_FSR2] & 0x7F)];
				if (regs[REG_POSTID] & BIT_FSR2PE) {
					if (regs[REG_POSTID] & BIT_FSR2ID) { //Auto Inc
						if (regs[REG_FSR2] == 0xFF)
							regs[REG_BSR2] ++; //carry
						regs[REG_FSR2] ++;
					}
					else { //Auto Dec
						if (regs[REG_FSR2] == 0x00)
							regs[REG_BSR2] --; //borrow
						regs[REG_FSR2] --;
					}
				}
				break;
			case REG_POSTID:
			case REG_LCDARH:
			case REG_LCDARL:
			case REG_LCDDAT:
			case REG_LCDCON:
				byte = lcd_read_byte(addr);
				break;
			default: byte = regs[addr]; break;
			}
			return byte;
		}
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
        ram[(mem_page<<7)|addr&0x7F] = byte;
    } else {
		if ((addr >= 0x25) && (addr <= 0x3F) && (regs[REG_CPUCON] & BIT_WBK)) {
			ram_wbk[addr - 0x25] = byte;
		}
		else {
			regs[addr] = byte;//Do all write
			switch (addr) {
			case REG_INDF0:
				if (regs[REG_FSR0] & 0x80) {
					ram[((regs[REG_BSR] & 0x3F) << 7) | (regs[REG_FSR0] & 0x7F)] = byte;
				}
				else {
					if (regs[REG_BSR] != 0) mmio_bad_read_byte(addr);
					regs[REG_FSR0] = byte;
				}
				if (regs[REG_POSTID] & BIT_FSR0PE) {
					if (regs[REG_POSTID] & BIT_FSR0ID) { //Auto Inc
						regs[REG_FSR0] ++;
					}
					else { //Auto Dec
						regs[REG_FSR0] --;
					}
				}
				break;
			case REG_INDF1:
				ram[((regs[REG_BSR1] & 0x3F) << 7) | (regs[REG_FSR1] & 0x7F)] = byte;
				if (regs[REG_POSTID] & BIT_FSR1PE) {
					if (regs[REG_POSTID] & BIT_FSR1ID) { //Auto Inc
						if (regs[REG_FSR1] == 0xFF)
							regs[REG_BSR1] ++; //carry
						regs[REG_FSR1] ++;
					}
					else { //Auto Dec
						if (regs[REG_FSR1] == 0x00)
							regs[REG_BSR1] --; //borrow
						regs[REG_FSR1] --;
					}
				}
				break;
			case REG_INDF2:
				ram[((regs[REG_BSR2] & 0x3F) << 7) | (regs[REG_FSR2] & 0x7F)] = byte;
				if (regs[REG_POSTID] & BIT_FSR2PE) {
					if (regs[REG_POSTID] & BIT_FSR2ID) { //Auto Inc
						if (regs[REG_FSR2] == 0xFF)
							regs[REG_BSR2] ++; //carry
						regs[REG_FSR2] ++;
					}
					else { //Auto Dec
						if (regs[REG_FSR2] == 0x00)
							regs[REG_BSR2] --; //borrow
						regs[REG_FSR2] --;
					}
				}
				break;
			case REG_POSTID:
			case REG_LCDARH:
			case REG_LCDARL:
			case REG_LCDDAT:
			case REG_LCDCON: lcd_write_byte(addr, byte);
				break;
			}
		}
    }
}

//Address:
//0000-1FFF: Banked memory plain address
uint8_t mmio_mem_read_byte(uint16_t addr) {
	return ram[addr];
}

void mmio_mem_write_byte(uint16_t addr, uint8_t byte) {
	ram[addr] = byte;
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
