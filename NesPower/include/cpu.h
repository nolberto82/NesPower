#pragma once

#include "types.h"

u8 a;
u8 y;
u8 x;
u8 ps;
u8 sp;
u16 pc;

u32 cpucycles;
u32 ppucycles;
u32 totalcycles;

bool pagecrossed;

bool fc;
bool fz;
bool fi;
bool fd;
bool fb;
bool fu;
bool fv;
bool fn;

bool app_running;

bool app_tracing;

FILE* ftrace;

void cpu_init(char* filename);

void cpu_clean();

void cpu_step();

void set_cpu_status();

void get_flags(char* flags);

void BRK();

void NMI();

void RTI();

void RTS();

void JSR(u16 addr);

void TSX();

void TXS();

void PLP();

void PLA();

void PHP();

void PHA();

void BCS(u16 v);

void BVS(u16 v);

void BEQ(u16 v);

void BPL(u16 v);

void BMI(u16 v);

void BNE(u16 v);

void BVC(u16 v);

void BCC(u16 v);

void BIT(u16 addr);

void AND();

void ANDM(u16 addr);

void DEC(u16 addr);

void INC(u16 addr);

void STY(u16 addr);

void STX(u16 addr);

void STA(u16 addr);

void LDY();

void LDYM(u16 addr);

void LDX();

void LDXM(u16 addr);

void CPY();

void CPYM(u16 addr);

void CPX();

void CPXM(u16 addr);

void CMP();

void CMPM(u16 addr);

void EOR();

void EORM(u16 addr);

void ORA();

void ORAM(u16 addr);

void SBC();

void SBCM(u16 addr);

void ADC();

void ADCM(u16 addr);

void LDA();

void LDAM(u16 addr);

void update_flags();

void DEY();

void DEX();

void INY();

void INX();

void TAY();

void TAX();

void TYA();

void TXA();

void ROL();

void ROLM(u16 addr);

void ASL();

void ASLM(u16 addr);

void ROR();

void RORM(u16 addr);

void LSR();

void LSRM(u16 addr);

void set_carry(u8 v);

void set_zero(u8 v);

void set_negative(u8 v);

void set_overflow(u8 v);

u8 get_imme();

u8 get_zerp();

u8 get_zerx();

u8 get_zery();

u16 get_abso();

u16 get_absx();

u16 get_absy();

u16 get_indx();

u16 get_indy();

u16 get_indi();

u16 get_rela();
