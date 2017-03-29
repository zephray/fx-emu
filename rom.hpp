//ePS6800 Mask ROM

#pragma once
#include "main.hpp"

void rom_init();

//Address: 17bit word(16-bit) address
uint16_t rom_read_word(uint32_t addr);

void rom_open(const char * filename);
