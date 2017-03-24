//ePS6800 Emulation Core

#pragma once
#include "main.hpp"
#include "eps.hpp"

void cpu_push(uint32_t dat);
uint32_t cpu_pop();
void cpu_loop(uint32_t count);
void cpu_reset();
void cpu_interrupt(uint32_t addr);
void cpu_interpret_instruction(uint32_t instr);
void *cpu_save_state(size_t *size);
void cpu_load_state(void *state);
