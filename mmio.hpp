//ePS6800 Register & Memory

#pragma once
#include "main.hpp"
#include "eps.hpp"

#define MEM_MAXSIZE 0x4000

#define RAM_FLAGS(memptr) (*(uint8_t *)((uint8_t *)(memptr) + MEM_MAXSIZE))

void mmio_bad_read_byte(uint8_t addr);
void mmio_bad_write_byte(uint8_t addr);

//Address:
//00-FF: Regular Register and Memory Bank 0
uint8_t mmio_read_byte(uint8_t addr);
void mmio_write_byte(uint8_t addr, uint8_t byte);

//Fast access to regs, used only for CPU status regs
void mmio_write_byte_internal(uint8_t addr, uint8_t byte);
uint8_t mmio_read_byte_internal(uint8_t addr);

//Address:
//0000-00FF: Regular memory one would expect
//0100-017F: RAM Bank 0
//0180-3FFF: Other RAM Banks
uint8_t mmio_mem_read_byte(uint16_t addr); 
void mmio_mem_write_byte(uint16_t addr, uint8_t byte);

void mmio_init();
void mmio_reset();
void *mmio_save_state(size_t *size);
void mmio_load_state(void *state);
