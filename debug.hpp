//ePS6800 Debug Related Functions
#pragma once
#include "main.hpp"

extern char *reg_names[];

char *disAsm(uint16_t addr, uint32_t instr);
