//ePS6800 Common
#pragma once

#define REG_INDF0   0x00
#define REG_FSR0    0x01
#define REG_BSR     0x02
#define REG_INDF1   0x03
#define REG_FSR1    0x04
#define REG_BSR1    0x05
#define REG_STKPTR  0x06
#define REG_PCL     0x07
#define REG_PCM     0x08
#define REG_PCH     0x09
#define REG_ACC     0x0A
#define REG_TABPTRL 0x0B
#define REG_TABPTRM 0x0C
#define REG_TABPTRH 0x0D
#define REG_LCDDAT  0x0E 
#define REG_STATUS  0x0F
#define REG_INDF2   0x10
#define REG_FSR2    0x11
#define REG_BSR2    0x12
#define REG_CPUCON  0x20
#define REG_POSTID  0x21
#define REG_LCDARL  0x22
#define REG_LCDARH  0x23
#define REG_LCDCON  0x2E

//Unknown behavior:
//What if you RPT a JUMP?