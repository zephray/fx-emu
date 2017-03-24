//ePS6800 Timer

//Event count mode not emulated

#include "timer.hpp"
#include "cpu.hpp"

static uint8_t timer_reg[0x30];//many are not used

uint32_t last_time = 0, current_time;
int32_t timer0_val, timer1_val, timer2_val; //time to next underflow in ms
int32_t timer0_rrl, timer1_rrl, timer2_rrl; //underflow interval

void timer_recalc_0() {
	float ttl;
	uint16_t prescaler0[] = { 1, 4, 16, 64 };
	uint16_t rrl;
	rrl = (uint16_t)timer_reg[REG_TRL0H] << 8 | timer_reg[REG_TRL0L];
	ttl = (timer_reg[REG_TR0CON] & BIT_T0CS) ? (FHOSC / 2) : (FOSC);
	ttl = (1 / ttl) * prescaler0[(timer_reg[REG_TR0CON]) & 0x03] * (rrl + 1);
	ttl *= 1000; //s -> ms
	timer1_rrl = (int32_t)ttl;
	timer1_val = timer1_rrl;
}

void timer_recalc_1() {
	float ttl;
	uint16_t prescaler1[] = { 4, 16, 64, 256 };
	uint16_t rrl;
	rrl = timer_reg[REG_TRL1];
	ttl = FOSC;
	ttl = (1 / ttl) * prescaler1[(timer_reg[REG_TR1CON]) & 0x03] * (rrl + 1);
	ttl *= 1000; //s -> ms
	timer1_rrl = (int32_t)ttl;
	timer1_val = timer1_rrl;
}

void timer_recalc_2() {
	float ttl;
	uint16_t prescaler2[] = { 1, 2, 4, 8 };
	uint16_t rrl;
	rrl = timer_reg[REG_TRL2];
	ttl = (timer_reg[REG_TR2WCON] & BIT_T2CS) ? (FHOSC / 2) : (FOSC);
	ttl = (1 / ttl) * prescaler2[(timer_reg[REG_TR0CON]) & 0x03] * (rrl + 1);
	ttl *= 1000; //s -> ms
	timer2_rrl = (int32_t)ttl;
	timer2_val = timer1_rrl;
}

uint8_t timer_read_byte(uint8_t addr) {
	uint8_t byte;
	if (addr < 0x30) {
		//switch (addr) {
		//default:
			byte = timer_reg[addr];
		//}
	}
	else {
		byte = 0xFF;
		mmio_bad_read_byte(addr);
	}
	return byte;
}

void timer_write_byte(uint8_t addr, uint8_t byte) {
	if (addr < 0x30) {
		timer_reg[addr] = byte;
		switch (addr) {
		case REG_TR0CON:
			timer_recalc_0();
			break;
		case REG_TR1CON:
			timer_recalc_1();
			break;
		case REG_TR2WCON:
			timer_recalc_2();
			break;
		}
	}
	else {
		mmio_bad_write_byte(addr);
	}
}

void timer_reset() {
	int i;
	for (i = 0; i < 0x30; i++) timer_reg[i] = 0x00;
	last_time = SDL_GetTicks();
}

void timer_tick() {
	uint32_t time_diff;
	current_time = SDL_GetTicks();
	time_diff = current_time - last_time;
	if (timer_reg[REG_TR0CON] & BIT_T0EN) {
		timer0_val -= time_diff;
		if (timer0_val <= 0) {
			//underflow
			timer0_val = timer0_rrl;
			if (timer_reg[REG_TR0CON] & BIT_TMR0IE) {
				timer_reg[REG_INTSTA] |= BIT_TMR0I;
				cpu_interrupt(ADDR_TIMINT);
			}
		}
	}
	if (timer_reg[REG_TR1CON] & BIT_T1EN) {
		timer1_val -= time_diff;
		if (timer1_val <= 0) {
			timer1_val = timer1_rrl;
			if (timer_reg[REG_TR1CON] & BIT_TMR1IE) {
				timer_reg[REG_INTSTA] |= BIT_TMR1I;
				cpu_interrupt(ADDR_TIMINT);
			}
		}
	}
	if (timer_reg[REG_TR2WCON] & BIT_T2EN) {
		timer0_val -= time_diff;
		if (timer0_val <= 0) {
			timer0_val = timer0_rrl;
			if (timer_reg[REG_TR2WCON] & BIT_TMR2IE) {
				timer_reg[REG_INTSTA] |= BIT_TMR2I;
				cpu_interrupt(ADDR_TIMINT);
			}
		}
	}
	last_time = current_time;
}

void *timer_save_state(size_t *size) {
	return NULL;
}

void timer_load_state(void *state) {

}
