//ePS6800 LCD

#include "lcd.hpp"
#include "mmio.hpp"

static uint8_t lcd_fb[128*4];
static uint8_t lcd_reg[0x30];//many are not used

uint8_t lcd_read_byte(uint8_t addr) {
    uint8_t byte;
    if (addr < 0x30) {
        switch (addr) {
            case REG_LCDDAT:
            byte = lcd_fb[(((uint16_t)(lcd_reg[REG_LCDARH]&0x03))<<7)|(lcd_reg[REG_LCDARL])];
            if (lcd_reg[REG_POSTID] & 0x04) { //LCDPE ON
                if (lcd_reg[REG_POSTID] & 0x40) { //Auto Increase
                    if (lcd_reg[REG_LCDARL] < 0x61) lcd_reg[REG_LCDARL]++;
                } else {
                    if (lcd_reg[REG_LCDARL] > 0x00) lcd_reg[REG_LCDARL]--;
                }
            }
            break;
            default:
            byte = lcd_reg[addr];
        }
    } else {
        byte = 0xFF;
        mmio_bad_read_byte(addr);
    }
    return byte;
}

void lcd_write_byte(uint8_t addr, uint8_t byte) {
    if (addr < 0x30) {
        lcd_reg[addr] = byte;
        switch (addr) {
            case REG_LCDDAT:
            lcd_fb[((lcd_reg[REG_LCDARH]&0x03)<<7)|(lcd_reg[REG_LCDARL])] = byte;
            if (lcd_reg[REG_POSTID] & 0x04) { //LCDPE ON
                if (lcd_reg[REG_POSTID] & 0x40) { //Auto Increase
					if (lcd_reg[REG_LCDARL] < 0x61) lcd_reg[REG_LCDARL]++; else { lcd_reg[REG_LCDARL] = 0; lcd_reg[REG_LCDARH]++; }
                } else {
                    if (lcd_reg[REG_LCDARL] > 0x00) lcd_reg[REG_LCDARL]--; else { lcd_reg[REG_LCDARL] = 0x61; lcd_reg[REG_LCDARH]--; }
                }
            }
            break;
            if (byte > 0x61) {
                lcd_reg[addr] = byte; 
                mmio_bad_write_byte(addr);
            }
            break;
        }
    } else {
        mmio_bad_write_byte(addr);
    }
}

uint8_t lcd_ram_read_byte(uint16_t addr) {
    return lcd_fb[(addr/96)*128+(addr%96)];
}

void lcd_ram_write_byte(uint16_t addr, uint8_t byte) {
    lcd_fb[(addr/96)*128+(addr%96)] = byte;
}

void lcd_reset() {
	int i;
	for (i = 0; i < 0x30; i++) lcd_reg[i] = 0;
}

void lcd_flush(SDL_Surface *surface) {
    uint32_t *pixels = (uint32_t *)surface->pixels;
    uint32_t pixel;
    for (int i = 0; i < 96; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 8; k++) {
                pixel = ((lcd_fb[j*128 + i] & (1 << k))?(PIX_ON):(PIX_OFF));
                pixels[(j*8+k)*96+i] = pixel;
            }
        }
    }
}

void *lcd_save_state(size_t *size) {
	return NULL;
}

void lcd_load_state(void *state) {

}