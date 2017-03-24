//ePS6800 Common
#pragma once

#define FOSC        32768
#define FHOSC       500000

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
#define REG_INTSTA  0x24
#define REG_TR0CON  0x25
#define REG_TRL0H   0x26
#define REG_TRL0L   0x27
#define REG_T0CH    0x28
#define REG_T0CL    0x29
#define REG_TR1CON  0x2A
#define REG_TRL1    0x2B
#define REG_TR2WCON 0x2C
#define REG_TRL2    0x2D
#define REG_LCDCON  0x2E

//POSTID
#define BIT_FSR0PE	0x01
#define BIT_FSR1PE  0x02
#define BIT_LCDPE   0x04
#define BIT_FSR2PE  0x08
#define BIT_FSR0ID  0x10
#define BIT_FSR1ID  0x20
#define BIT_LCDID   0x40
#define BIT_FSR2ID  0x80

//CPUCON
#define BIT_WBK		0x80
#define BIT_GLINT	0x04
#define BIT_MS1		0x02
#define BIT_MS0		0x01

//TR0CON
#define BIT_T0ENMD  0x20
#define BIT_TMR0IE  0x10
#define BIT_T0EN    0x08
#define BIT_T0CS    0x04
#define BIT_T0PSR1  0x02
#define BIT_T0PSR0  0x01

//TR1CON
#define BIT_T1WKEN  0x80
#define BIT_TMR1IE  0x10
#define BIT_T1EN    0x08
#define BIT_T1PSR1  0x02
#define BIT_T1PSR0  0x01

//TR2WCON
#define BIT_WDTEN	0x80
#define BIT_WDTPSR1 0x40
#define BIT_WDTPSR0 0x20
#define BIT_TMR2IE  0x10
#define BIT_T2EN    0x08
#define BIT_T2CS    0x04
#define BIT_T2PSR1  0x02
#define BIT_T2PSR0  0x01

//INTSTA
#define BIT_TMR2I   0x04
#define BIT_TMR1I   0x02
#define BIT_TMR0I   0x01

#define ADDR_PAINT  0x00000002
#define ADDR_TIMINT 0x00000008

//Unknown behavior:
//What if you RPT a JUMP?