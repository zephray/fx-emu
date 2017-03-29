#pragma once
#include "main.hpp"
#include "eps.hpp"

uint8_t kbd_read_byte(uint8_t addr);
void kbd_write_byte(uint8_t addr, uint8_t byte);

void kbd_keydown(uint8_t key);
void kbd_keyup(uint8_t key);
void kbd_ondown();
void kbd_onup();
void kbd_reset();
void *kbd_save_state(size_t *size);
void kbd_load_state(void *state);
