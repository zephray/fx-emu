#pragma once
#include "main.hpp"
#include "eps.hpp"

//Address:
//00-FF: Regular Register Address
uint8_t timer_read_byte(uint8_t addr);
void timer_write_byte(uint8_t addr, uint8_t byte);

void time_tick();
void timer_reset();
void *timer_save_state(size_t *size);
void timer_load_state(void *state);
