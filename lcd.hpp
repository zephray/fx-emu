//ePS6800 LCD

#pragma once
#include "main.hpp"
#include "eps.hpp"

const uint32_t PIX_ON  = 0x00000000;
const uint32_t PIX_OFF = 0x00FFFFFF;

//Address:
//00-FF: Regular Register Address
uint8_t lcd_read_byte(uint8_t addr);
void lcd_write_byte(uint8_t addr, uint8_t byte);

//Address:
//000-17F: LCD Framebuffer Address
uint8_t lcd_ram_read_byte(uint16_t addr);
void lcd_ram_write_byte(uint16_t addr, uint8_t byte);

void lcd_flush(SDL_Surface *surface);
void *lcd_save_state(size_t *size);
void lcd_load_state(void *state);
