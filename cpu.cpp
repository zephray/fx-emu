//ePS6800 Emulation Core
#include "cpu.hpp"
#include "mmio.hpp"
#include "rom.hpp"

enum cpu_mode { MODE_SLOW, MODE_FAST, MODE_IDLE, MODE_SLEEP};

uint8_t status; //r0fh
uint32_t pc; //program counter
uint32_t stack[32];//very niubility stack
uint8_t rpt_counter; //rpt counter
enum cpu_mode mode;

void status_zero(uint8_t x) { if (((x)) == 0) status |= 0x04; else status &= (~0x04); }
void status_carry(uint8_t x) { if ((x)) status |= 0x01; else status &= (~0x01); }
void status_ov(uint8_t x)  { if ((x)) status |= 0x08; else status &= (~0x08); }
void status_sge(uint8_t x)  { if ((x)) status |= 0x20; else status &= (~0x20); }
void status_sle(uint8_t x)  { if ((x)) status |= 0x10; else status &= (~0x10); }

#define status_ov_add(op1, op2, result) status_ov(((op1) ^ (result)) & ((op2) ^ (result)) & 0x80)
#define status_ov_sub(op1, op2, reuslt) status_ov(((op1) ^ (op2)) & ((op1) ^ (result)) & 0x80)

void cpu_push(uint32_t dat) {
    uint8_t stkptr;
    stkptr = mmio_read_byte_internal(REG_STKPTR);
    stack[stkptr] = dat;
    if (stkptr < 32)
        stkptr++;
    else 
        stkptr=0;
    mmio_write_byte_internal(REG_STKPTR, stkptr);
}

uint32_t cpu_pop() {
    uint8_t stkptr;
    stkptr = mmio_read_byte_internal(REG_STKPTR);
    if (stkptr > 0)
        stkptr--;
	mmio_write_byte_internal(REG_STKPTR, stkptr);
    return stack[stkptr];
}

void cpu_loop(uint32_t count) {
    int32_t cnt;
    uint32_t instr;
    
    cnt = count;
    while ((cnt!=0)&&((mode==MODE_SLOW)||(mode==MODE_FAST))) {
        instr = ((rom_read_word(pc) << 16) | rom_read_word(pc+1));
        cpu_interpret_instruction(instr);
        cnt --;
    }
}

void cpu_reset() {
    pc = 0;
    status = 0;
    rpt_counter = 0;
    mode = MODE_FAST;
    mmio_reset();
}

void cpu_interrupt(uint32_t addr) {
	uint8_t cpucon;
	cpucon = mmio_read_byte_internal(REG_CPUCON);
	if (cpucon & BIT_GLINT) {
		cpu_push(pc);
		pc = addr;
		cpucon &= ~(BIT_GLINT);
		mmio_write_byte_internal(REG_CPUCON);
	}
}

uint8_t alu_add(uint8_t a, uint8_t b) {
    uint8_t result;
    
    result = a + b;
    status_zero(result);
    status_carry(((uint16_t)a+(uint16_t)b)&0x100);
    status_ov_add(a, b, result);
    status_sle(!(result&0x80));//Working weird
    status_sge(result&0x80);
    
    return result;
}

uint8_t alu_adc(uint8_t a, uint8_t b, uint8_t c) {
    uint8_t result;
    
    result = a + b + c;
    status_zero(result);
    status_carry(((uint16_t)a+(uint16_t)b+(uint16_t)c)&0x100);
    status_ov(((uint16_t)a+(uint16_t)b+(uint16_t)c)>0x7F);
    status_sle(!(result&0x80));//Working weird
    status_sge(result&0x80);
    
    return result;
}

uint8_t alu_sub(uint8_t a, uint8_t b) {
    uint8_t result;

    result = a - b;
    status_zero(result);
    status_carry(!(a<b));
    status_ov_sub(a, b, result);
    status_sle(!(result&0x80));//Working weird
    status_sge(result&0x80);

    return result;
}

uint8_t alu_subb(uint8_t a, uint8_t b, uint8_t c) {
    uint8_t result;

    result = a - b - c;
    status_zero(result);
    status_carry(!(a<(b+c)));
    status_ov(((int16_t)a-(int16_t)b-(int16_t)c)<(0-0x7F));
    status_sle(!(result&0x80));//Working weird
    status_sge(result&0x80);

    return result;
}

//BCD Addition
uint8_t alu_adddc(uint8_t a, uint8_t b, uint8_t c) {
    uint8_t a1, b1, s1;

    a1 = ( a >> 4 ) * 10 + ( a & 0xFF );
    b1 = ( b >> 4 ) * 10 + ( b & 0xFF );
    
    s1 = a1 + b1 + c;
    status_carry((s1 > 99));

    return (s1/10) << 4 | (s1%10);
}

uint8_t alu_subdb(uint8_t a, uint8_t b, uint8_t c) {
    uint8_t a1, b1;
    uint8_t s1;

    a1 = ( a >> 4 ) * 10 + ( a & 0xFF );
    b1 = ( b >> 4 ) * 10 + ( b & 0xFF );
    
    s1 = a1 - b1 - c;
    status_carry(!(a1<(b1+c)));

    return (s1/10) << 4 | (s1%10);
}

void cpu_interpret_instruction(uint32_t instr) {
    uint16_t instr1;
    uint16_t imm16_1;
	uint8_t imm8_1, imm8_2;
	uint32_t imm32_1;
    uint32_t newpc, mempc;
    uint8_t  temp8_1, temp8_2, temp8_3;  //instruction related temp
    uint16_t temp16; //instruction related temp
    uint32_t temp32; //instruction related temp
    

    instr1 = instr >> 16;
    newpc = pc + 1;
    status = mmio_read_byte_internal(REG_STATUS);

    if (rpt_counter != 0) {
        newpc = pc;
        rpt_counter --;
    }

    switch (instr1 & 0xFFFF) {
        case 0x0000: break;  //nop
        case 0x0001: break;  //not implemented
        case 0x0002: temp8_1 = mmio_read_byte_internal(REG_CPUCON);
                     if (temp8_1 & 0x02) mode = MODE_IDLE;
                     else mode = MODE_SLEEP;
                     break;  //slep
        case 0x2BFE: temp8_1 = mmio_read_byte_internal(REG_CPUCON);
                     temp8_1 |= 0x04; 
                     mmio_write_byte_internal(REG_CPUCON, temp8_1);
                     //reti, enable interrupt and run through
        case 0x2BFF: newpc = cpu_pop();
                     break;//ret
        default: //no match
        switch (instr1 & 0xFFFE) {
            case 0x0030: cpu_push(newpc); //run through
            case 0x0020: imm32_1 = instr & 0x0001FFFF;
                         newpc = imm32_1;
                         break;
            default: //no match again
            imm8_1 = instr1 & 0x00FF; //just assume
			imm16_1 = imm16_1 = instr & 0x0000FFFF; //important: only change low 16 bits of PC
			imm8_2 = (instr1 & 0x0700) >> 8; //used in JBC/JBS/BC/BS/BTG
            switch (instr1 & 0xFF00) {
                case 0x2000: mmio_write_byte_internal(REG_ACC, mmio_read_byte(imm8_1));
                             break;//mov a, r
                case 0x2100: mmio_write_byte(imm8_1, mmio_read_byte_internal(REG_ACC));
                             break;//mov r, a
                case 0x4E00: mmio_write_byte_internal(REG_ACC, imm8_1);
                             break;//mov r, #
                case 0x2500: status_zero(mmio_read_byte(imm8_1));
                             break;//test r
                case 0x2700: rpt_counter = mmio_read_byte(imm8_1);
                             break;//rpt r
                case 0x8300: mmio_write_byte_internal(REG_BSR, imm8_1);
                             break;//bank #
                case 0x2400: mmio_write_byte(imm8_1, 0);
                             break;//clr r
                case 0x4000: mmio_write_byte_internal(REG_TABPTRL, imm8_1);
                             break;//tbptl #
                case 0x4100: mmio_write_byte_internal(REG_TABPTRM, imm8_1);
                             break;//tbptm #
                case 0x4200: mmio_write_byte_internal(REG_TABPTRH, imm8_1);
                             break;//tbpth #
                case 0x2f00: temp32 = (mmio_read_byte_internal(REG_TABPTRH) << 16) |
                                      (mmio_read_byte_internal(REG_TABPTRM) << 8)  |
                                       mmio_read_byte_internal(REG_TABPTRL);
                             temp32 += mmio_read_byte_internal(REG_ACC);
                             temp16 = rom_read_word(temp32 >> 1);
                             if (temp32&0x01) temp16 >>= 8;
                             mmio_write_byte(imm8_1, temp16&0xff);
                             break;//tbrd a, r
                case 0x0200: temp8_1 = mmio_read_byte_internal(REG_ACC);
                             temp8_1 |= mmio_read_byte(imm8_1);
                             status_zero(temp8_1);
                             mmio_write_byte_internal(REG_ACC, temp8_1);
                             break;//or a, r
                case 0x0300: temp8_1 = mmio_read_byte_internal(REG_ACC);
                             temp8_1 |= mmio_read_byte(imm8_1);
                             status_zero(temp8_1);
                             mmio_write_byte(imm8_1, temp8_1);
                             break;//or r, a
                case 0x4400: temp8_1 = mmio_read_byte_internal(REG_ACC);
                             temp8_1 |= imm8_1;
                             status_zero(temp8_1);
                             mmio_write_byte_internal(REG_ACC, temp8_1);
                             break;//or a, #
                case 0x0400: temp8_1 = mmio_read_byte_internal(REG_ACC);
                             temp8_1 &= mmio_read_byte(imm8_1);
                             status_zero(temp8_1);
                             mmio_write_byte_internal(REG_ACC, temp8_1);
                             break;//and a, r
                case 0x0500: temp8_1 = mmio_read_byte_internal(REG_ACC);
                             temp8_1 &= mmio_read_byte(imm8_1);
                             status_zero(temp8_1);
                             mmio_write_byte(imm8_1, temp8_1);
                             break;//and r, a
                case 0x4500: temp8_1 = mmio_read_byte_internal(REG_ACC);
                             temp8_1 &= imm8_1;
                             status_zero(temp8_1);
                             mmio_write_byte_internal(REG_ACC, temp8_1);
                             break;//and a, #
                case 0x0600: temp8_1 = mmio_read_byte_internal(REG_ACC);
                             temp8_1 ^= mmio_read_byte(imm8_1);
                             status_zero(temp8_1);
                             mmio_write_byte_internal(REG_ACC, temp8_1);
                             break;//xor a, r
                case 0x0700: temp8_1 = mmio_read_byte_internal(REG_ACC);
                             temp8_1 ^= mmio_read_byte(imm8_1);
                             status_zero(temp8_1);
                             mmio_write_byte(imm8_1, temp8_1);
                             break;//xor r, a
                case 0x4600: temp8_1 = mmio_read_byte_internal(REG_ACC);
                             temp8_1 ^= imm8_1;
                             status_zero(temp8_1);
                             mmio_write_byte_internal(REG_ACC, temp8_1);
                             break;//xor a, #
                case 0x0800: temp8_1 = mmio_read_byte(imm8_1);
                             temp8_1 = ~temp8_1; 
                             status_zero(temp8_1);
                             mmio_write_byte_internal(REG_ACC, temp8_1);
                             break;//coma r
                case 0x0900: temp8_1 = mmio_read_byte(imm8_1);
                             temp8_1 = ~temp8_1; 
                             status_zero(temp8_1);
                             mmio_write_byte(imm8_1, temp8_1);
                             break;//com r        
                case 0x1C00: temp8_1 = mmio_read_byte(imm8_1);
                             status_carry(((uint16_t)temp8_1+1)&0x100);
                             temp8_1 += 1; 
                             status_zero(temp8_1);
                             mmio_write_byte_internal(REG_ACC, temp8_1);
                             break;//inca r
                case 0x1D00: temp8_1 = mmio_read_byte(imm8_1);
                             status_carry(((uint16_t)temp8_1+1)&0x100);
                             temp8_1 += 1; 
                             status_zero(temp8_1);
                             mmio_write_byte(imm8_1, temp8_1);
                             break;//inc r
                case 0x1000: mmio_write_byte_internal(REG_ACC, 
                                alu_add(mmio_read_byte(imm8_1), mmio_read_byte_internal(REG_ACC)));
                             break;//add a, r
                case 0x1100: mmio_write_byte(imm8_1, 
                                alu_add(mmio_read_byte(imm8_1), mmio_read_byte_internal(REG_ACC)));
                             break;//add r, a
                case 0x4A00: mmio_write_byte_internal(REG_ACC, 
                                alu_add(imm8_1, mmio_read_byte_internal(REG_ACC)));
                             break;//add a, #
                case 0x1200: mmio_write_byte_internal(REG_ACC, 
                                alu_adc(mmio_read_byte(imm8_1), mmio_read_byte_internal(REG_ACC),
                                status&0x01));
                             break;//adc a, r
                case 0x1300: mmio_write_byte_internal(imm8_1, 
                                alu_adc(mmio_read_byte(imm8_1), mmio_read_byte_internal(REG_ACC),
                                status&0x01));
                             break;//adc r, a
                case 0x4B00: mmio_write_byte_internal(REG_ACC, 
                                alu_adc(imm8_1, mmio_read_byte_internal(REG_ACC),
                                status&0x01));
                             break;//adc a, #
                case 0x1E00: temp8_1 = mmio_read_byte(imm8_1);
                             status_carry(!(imm8_1<1));
                             temp8_1 -= 1; 
                             status_zero(temp8_1);
                             mmio_write_byte_internal(REG_ACC, temp8_1);
                             break;//deca r
                case 0x1F00: temp8_1 = mmio_read_byte(imm8_1);
                             status_carry(!(imm8_1<1));
                             temp8_1 -= 1; 
                             status_zero(temp8_1);
                             mmio_write_byte(imm8_1, temp8_1);
                             break;//dec r
                case 0x1600: mmio_write_byte_internal(REG_ACC,
                                alu_sub(mmio_read_byte(imm8_1), mmio_read_byte_internal(REG_ACC)));
                             break;//sub a, r            
                case 0x1700: mmio_write_byte(imm8_1,
                                alu_sub(mmio_read_byte(imm8_1), mmio_read_byte_internal(REG_ACC)));
                             break;//sub r, a
                case 0x4C00: mmio_write_byte_internal(REG_ACC,
                                alu_sub(imm8_1, mmio_read_byte_internal(REG_ACC)));
                             break;//sub a, #         
                case 0x1800: mmio_write_byte_internal(REG_ACC,
                                alu_subb(mmio_read_byte(imm8_1), mmio_read_byte_internal(REG_ACC),
                                ((status&0x01)?0:1)));
                             break;//subb a, r            
                case 0x1900: mmio_write_byte(imm8_1,
                                alu_subb(mmio_read_byte(imm8_1), mmio_read_byte_internal(REG_ACC),
                                ((status&0x01)?0:1)));
                             break;//subb r, a
                case 0x4D00: mmio_write_byte_internal(REG_ACC,
                                alu_subb(imm8_1, mmio_read_byte_internal(REG_ACC),
                                ((status&0x01)?0:1)));
                             break;//subb a, #         
                case 0x1400: mmio_write_byte_internal(REG_ACC,
                                alu_adddc(mmio_read_byte(imm8_1), mmio_read_byte_internal(REG_ACC),
                                status&0x01));
                             break;//adddc a, r
                case 0x1500: mmio_write_byte(imm8_1,
                                alu_adddc(mmio_read_byte(imm8_1), mmio_read_byte_internal(REG_ACC),
                                status&0x01));
                             break;//adddc r, a
                case 0x1A00: mmio_write_byte_internal(REG_ACC,
                                alu_subdb(mmio_read_byte(imm8_1), mmio_read_byte_internal(REG_ACC),
                                ((status&0x01)?0:1)));
                             break;//subdb a, r
                case 0x1B00: mmio_write_byte(imm8_1,
                                alu_subdb(mmio_read_byte(imm8_1), mmio_read_byte_internal(REG_ACC),
                                ((status&0x01)?0:1)));
                             break;//subdb r,a
                case 0x0A00: temp8_1 = mmio_read_byte(imm8_1);
                             temp8_2 = temp8_1 & 0x01;
                             temp8_1 >>= 1;
                             temp8_1 |= (status&0x01) << 7;
                             status_carry(temp8_2);
                             mmio_write_byte_internal(REG_ACC, temp8_1);
                             break;//rrca r
                case 0x0B00: temp8_1 = mmio_read_byte(imm8_1);
                             temp8_2 = temp8_1 & 0x01;
                             temp8_1 >>= 1;
                             temp8_1 |= (status&0x01) << 7;
                             status_carry(temp8_2);
                             mmio_write_byte(imm8_1, temp8_1);
                             break;//rrc r
                case 0x0C00: temp8_1 = mmio_read_byte(imm8_1);
                             temp8_2 = temp8_1 & 0x80;
                             temp8_1 <<= 1;
                             temp8_1 |= status&0x01;
                             status_carry(temp8_2);
                             mmio_write_byte_internal(REG_ACC, temp8_1);
                             break;//rlca r
                case 0x0D00: temp8_1 = mmio_read_byte(imm8_1);
                             temp8_2 = temp8_1 & 0x80;
                             temp8_1 <<= 1;
                             temp8_1 |= status&0x01;
                             status_carry(temp8_2);
                             mmio_write_byte(imm8_1, temp8_1);
                             break;//rlc r
                case 0x2200: temp8_1 = mmio_read_byte(imm8_1);
                             temp8_1 >>= 1;
                             temp8_1 |= (status&0x01) << 7;
                             mmio_write_byte_internal(REG_ACC, temp8_1);
                             break;//shra r
                case 0x2300: temp8_1 = mmio_read_byte(imm8_1);
                             temp8_1 <<= 1;
                             temp8_1 |= status&0x01;
                             mmio_write_byte_internal(REG_ACC, temp8_1);
                             break;//shla r
                case 0x5400: temp8_1 = mmio_read_byte_internal(REG_ACC);
                             mmio_write_byte_internal(REG_ACC, mmio_read_byte(imm8_1));
                             mmio_write_byte(imm8_1, temp8_1);
                             break;//ex r
                case 0x5200: temp8_1 = mmio_read_byte_internal(REG_ACC);
                             temp8_2 = mmio_read_byte(imm8_1);
                             temp8_3 = temp8_1;
                             temp8_1 &= 0xF0;
                             temp8_1 |= temp8_2 & 0x0F;
                             temp8_2 &= 0xF0;
                             temp8_2 |= temp8_3 & 0x0F;
                             mmio_write_byte(imm8_1, temp8_2);
                             mmio_write_byte_internal(REG_ACC, temp8_1);
                             break;//exl r
                case 0x5300: temp8_1 = mmio_read_byte_internal(REG_ACC);
                             temp8_2 = mmio_read_byte(imm8_1);
                             temp8_3 = temp8_1;
                             temp8_1 &= 0x0F;
                             temp8_1 |= temp8_2 & 0xF0;
							 temp8_2 &= 0x0F;
							 temp8_2 |= temp8_3 & 0xF0;
                             mmio_write_byte(imm8_1, temp8_2);
                             mmio_write_byte_internal(REG_ACC, temp8_1);
                             break;//exh r
                case 0x2600: temp8_1 = mmio_read_byte(imm8_1);
                             temp8_1 &= 0xF0;
                             temp8_1 |= mmio_read_byte_internal(REG_ACC) & 0xF;
                             mmio_write_byte(imm8_1, temp8_1);
                             break;//movl r, a
                case 0x2800: temp8_1 = mmio_read_byte(imm8_1);
                             temp8_1 &= 0x0F;
                             temp8_1 |= mmio_read_byte_internal(REG_ACC) << 4;
                             mmio_write_byte(imm8_1, temp8_1);
                             break;//movh r, a
                case 0x2900: mmio_write_byte_internal(REG_ACC, mmio_read_byte(imm8_1)&0x0F);
                             break;//movl a, r
                case 0x2A00: mmio_write_byte_internal(REG_ACC, mmio_read_byte(imm8_1)>>4);
                             break;//movh a, r
				case 0x0100: temp8_1 = mmio_read_byte_internal(REG_ACC);
							 temp8_2 = mmio_read_byte(imm8_1);
							 mmio_write_byte_internal(REG_ACC, (temp8_1 & 0xF0) | (temp8_2 & 0x0F));
							 temp8_2 >>= 4;
							 temp8_2 |= temp8_1 << 4;
							 mmio_write_byte(imm8_1, temp8_2);
							 break;//sfr4 r
				case 0x4F00: temp8_1 = mmio_read_byte_internal(REG_ACC);
							 temp8_2 = mmio_read_byte(imm8_1);
							 mmio_write_byte_internal(REG_ACC, (temp8_1 & 0xF0) | (temp8_2 >> 4));
							 temp8_2 <<= 4;
							 temp8_2 |= temp8_1 & 0x0F;
							 break;//sfl4 r
				case 0x0F00: temp8_1 = mmio_read_byte(imm8_1);
							 mmio_write_byte(imm8_1, ((temp8_1 >> 4) | (temp8_1 << 4)));
							 break;//swap r
				case 0x0E00: temp8_1 = mmio_read_byte(imm8_1);
							 mmio_write_byte_internal(REG_ACC, ((temp8_1 >> 4) | (temp8_1 << 4)));
							 break;//swapa r
				case 0x5000: temp8_1 = mmio_read_byte(imm8_1) - 1;
							 if (temp8_1 != 0) { newpc &= 0xFFFF0000; newpc |= imm16_1; }
							 mmio_write_byte_internal(REG_ACC, temp8_1);
					         break;//jdnz a, r, addr
				case 0x5100: temp8_1 = mmio_read_byte(imm8_1) - 1;
							 if (temp8_1 != 0) { newpc &= 0xFFFF0000; newpc |= imm16_1; }
							 mmio_write_byte(imm8_1, temp8_1);
							 break;//jdnz r, addr
				case 0x4700: if (mmio_read_byte_internal(REG_ACC) >= imm8_1) { newpc &= 0xFFFF0000; newpc |= imm16_1; }
							 break;//jge a, #k, addr
				case 0x4800: if (mmio_read_byte_internal(REG_ACC) <= imm8_1) { newpc &= 0xFFFF0000; newpc |= imm16_1; }
							 break;//jle a, #k, addr
				case 0x4900: if (mmio_read_byte_internal(REG_ACC) == imm8_1) { newpc &= 0xFFFF0000; newpc |= imm16_1; }
							 break;//je a, #k, addr
				case 0x5500: if (mmio_read_byte_internal(REG_ACC) >= mmio_read_byte(imm8_1)) { newpc &= 0xFFFF0000; newpc |= imm16_1; }
							 break;//jge a, r, addr
				case 0x5600: if (mmio_read_byte_internal(REG_ACC) <= mmio_read_byte(imm8_1)) { newpc &= 0xFFFF0000; newpc |= imm16_1; }
							 break;//jle a, r, addr
				case 0x5700: if (mmio_read_byte_internal(REG_ACC) == mmio_read_byte(imm8_1)) { newpc &= 0xFFFF0000; newpc |= imm16_1; }
							 break;//je a, r, addr
				default: //no match (OP CODE MASK 0xFF)
					if ((instr1 & 0xFC00) == 0x2C00) { //OP CODE MASK 0xFC, TBRD opt, r
						temp32 = (mmio_read_byte_internal(REG_TABPTRH) << 16) |
							(mmio_read_byte_internal(REG_TABPTRM) << 8) |
							mmio_read_byte_internal(REG_TABPTRL);
						temp16 = rom_read_word(temp32 >> 1);
						if (temp32 & 0x01) temp16 >>= 8;
						mmio_write_byte(imm8_1, temp16 & 0xff);
						if ((instr1 & 0x0300) == 0x0100)
							temp32++;
						else if ((instr1 & 0x0300) == 0x0200)
							temp32--;
						mmio_write_byte_internal(REG_TABPTRH, (temp32 >> 16) & 0xFF);
						mmio_write_byte_internal(REG_TABPTRM, (temp32 >> 8) & 0xFF);
						mmio_write_byte_internal(REG_TABPTRL, temp32 & 0xFF);
					}
					else { //no match (OP CODE MASK 0xFC)
						switch (instr1 & 0xF800) {
							case 0x6800: mmio_write_byte(imm8_1, (mmio_read_byte(imm8_1) & ~(1 << (imm8_2))));
										 break; //bc r, b
							case 0x7000: mmio_write_byte(imm8_1, (mmio_read_byte(imm8_1) | (1 << (imm8_2))));
										 break; //bs r, b
							case 0x7800: temp8_1 = mmio_read_byte(imm8_1);
										 if (temp8_1 & (1 << imm8_2))
											temp8_1 &= ~(1 << (imm8_2));
										 else
											temp8_1 |= (1 << (imm8_2));
										 mmio_write_byte(imm8_1, temp8_1);
										 break; //btg r, b
							case 0x5800: if (!(mmio_read_byte(imm8_1) & (1 << imm8_2))) { newpc &= 0xFFFF0000; newpc |= imm16_1; }
										 break; //jbc r, b, addr
							case 0x6000: if (!(mmio_read_byte(imm8_1) & (1 << imm8_2))) { newpc &= 0xFFFF0000; newpc |= imm16_1; }
										 break; //jbc r, b, addr
							default: //no match (OP CODE MASK 0xF8)
								if ((instr1 & 0xF000) == 0x3000) { //OP CODE MASK 0xF0 S0CALL
									cpu_push(newpc);
									newpc = instr1 & 0x0FFF;
								}
								else { //no match (OP CODE MASK 0xF0)
									switch (instr1 & 0xE000) {
										case 0xE000: cpu_push(newpc); //scall, run through
										case 0xC000: newpc &= 0xFFFFE000;
													 newpc |= (instr1 & 0x1FFF);
													 break; //sjmp
										case 0x8000: mmio_write_byte(((instr1 & 0x1F00) >> 8), mmio_read_byte(instr1 & 0x00FF));
													 break; //movrp
										case 0xA000: mmio_write_byte((instr1 & 0x00FF), mmio_read_byte((instr1 & 0x1F00) >> 8));
													 break; //movpr
										default: //INVAILD INSTRUCTION
											printf("[Warning] Invaild instruction @ %4xh!\n", pc);
											break;
									}
								}
								break;
						}
					}
					break;
            }
            break;
        }
        break;
    }
	
	mempc = (uint32_t)mmio_read_byte_internal(REG_PCH) << 16;
	mempc |= (uint32_t)mmio_read_byte_internal(REG_PCM) << 8;
	mempc |= (uint32_t)mmio_read_byte_internal(REG_PCL);
	if (mempc == pc) { //Not changed during instruction execution
		pc = newpc;
		mmio_write_byte_internal(REG_PCL, pc & 0xFF);
		mmio_write_byte_internal(REG_PCM, (pc >> 8) & 0xFF);
		mmio_write_byte_internal(REG_PCH, (pc >> 16) & 0xFF);
	}
	else {
		pc = mempc; //Load PC with PC in the register set
	}
    mmio_write_byte_internal(REG_STATUS, status);
    
}

void *cpu_save_state(size_t *size) {
	return NULL;
}

void cpu_load_state(void *state) {
	
}
