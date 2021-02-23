#include "../include/cpu.h"
#include "../include/ppu.h"
#include "../include/mapper.h"
#include "../include/tracer.h"

#define CYCLES_PER_FRAME 341

u8 cyclestable[] =
{
	7,6,0,0,0,3,5,0,3,2,2,0,0,4,6,0,
	2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0,
	6,6,0,0,3,3,5,0,4,2,2,0,4,4,6,6,
	2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0,
	6,6,0,0,0,3,5,0,3,2,2,0,3,4,6,0,
	2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0,
	6,6,0,0,0,3,5,0,4,2,2,0,5,4,6,0,
	2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0,
	0,6,0,0,3,3,3,0,2,0,2,0,4,4,4,0,
	2,6,0,0,4,4,4,0,2,5,2,0,0,5,0,0,
	2,6,2,0,3,3,3,0,2,2,2,0,4,4,4,0,
	2,5,0,0,4,4,4,0,2,4,2,0,4,4,4,0,
	2,6,0,0,3,3,5,0,2,2,2,0,4,4,6,0,
	2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0,
	2,6,0,0,3,3,5,0,2,2,2,0,4,4,6,0,
	2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0
};

void cpu_init(char* filename)
{

	pc = cpu_read_word(0xfffc);

	if (filename == "tests/nestest.nes")
	{
		//pc = 0xc000;
	}

	sp = 0xfd;
	ps = 0x04;
	fi = true;
	fu = false;
	cpucycles = 0;
	totalcycles = 0;
	app_running = true;
	//app_tracing = true;

	if (app_tracing)
	{
		ftrace = fopen("tracenes.log", "w");
		char header[32] = { "FCEUX 2.2.3 - Trace Log File\n" };
		fwrite(header, 1, strlen(header), ftrace);
	}
}

void cpu_clean()
{
	if (app_tracing)
	{
		fclose(ftrace);
		app_tracing = false;
	}
}

void cpu_step()
{
	//printf("%02X\n", ram[0xd4]);
	while (ppucycles < CYCLES_PER_FRAME)
	{
		if (pc == 0xc089)
		{
			int yu = 0;
		}

		u8 op = ram[pc++];

		if (app_tracing)
		{
			char strtrace[256];

			trace_disasm(pc - 1, op, x, y, sp, totalcycles, strtrace);

			fwrite(strtrace, 1, strlen(strtrace), ftrace);
			sprintf(strtrace, "A:%02X X:%02X Y:%02X S:%02X", a, x, y, sp);
			fwrite(strtrace, 1, strlen(strtrace), ftrace);
			char strflags[20] = { 0 };
			get_flags(strflags);
			fwrite(strflags, 1, strlen(strflags), ftrace);
		}

		switch (op)
		{
			//Implied
		case 0x2a: { ROL(); } break;
		case 0x0a: { ASL(); } break;
		case 0x6a: { ROR(); } break;
		case 0x4a: { LSR(); } break;

		case 0x8a: { TXA(); } break;
		case 0x98: { TYA(); } break;
		case 0xaa: { TAX(); } break;
		case 0xa8: { TAY(); } break;

		case 0xe8: { INX(); } break;
		case 0xc8: { INY(); } break;
		case 0xca: { DEX(); } break;
		case 0x88: { DEY(); } break;

		case 0x38: { fc = true; ps |= 0x01; } break;
		case 0x18: { fc = false; ps &= ~0x01; } break;
		case 0xf8: { fd = true; ps |= 0x08; } break;
		case 0xd8: { fd = false; ps &= ~0x08; } break;
		case 0x78: { fi = true; ps |= 0x04; } break;
		case 0x58: { fi = false; ps &= ~0x04; } break;
		case 0xb8: { fv = false; ps &= ~0x40; } break;

		case 0xea: { } break;

			//Immediate
		case 0xa9: { LDA(); pc++; } break;
		case 0xa2: { LDX(); pc++; } break;
		case 0xa0: { LDY(); pc++; } break;

		case 0x69: { ADC(); pc++; } break;
		case 0xe9: { SBC(); pc++; } break;
		case 0x09: { ORA(); pc++; } break;
		case 0x29: { AND(); pc++; } break;
		case 0x49: { EOR(); pc++; } break;

		case 0xc9: { CMP(); pc++; } break;
		case 0xe0: { CPX(); pc++; } break;
		case 0xc0: { CPY(); pc++; } break;

			//Zero Page
		case 0xa5: { LDAM(get_zerp()); pc++; } break;
		case 0xa6: { LDXM(get_zerp()); pc++; } break;
		case 0xa4: { LDYM(get_zerp()); pc++; } break;

		case 0x85: { STA(get_zerp()); pc++; } break;
		case 0x86: { STX(get_zerp()); pc++; } break;
		case 0x84: { STY(get_zerp()); pc++; } break;

		case 0xe6: { INC(get_zerp()); pc++; } break;
		case 0xc6: { DEC(get_zerp()); pc++; } break;
		case 0x06: { ASLM(get_zerp()); pc++; } break;
		case 0x46: { LSRM(get_zerp()); pc++; } break;
		case 0x26: { ROLM(get_zerp()); pc++; } break;
		case 0x66: { RORM(get_zerp()); pc++; } break;

		case 0x65: { ADCM(get_zerp()); pc++; } break;
		case 0xe5: { SBCM(get_zerp()); pc++; } break;
		case 0x05: { ORAM(get_zerp()); pc++; } break;
		case 0x25: { ANDM(get_zerp()); pc++; } break;
		case 0x45: { EORM(get_zerp()); pc++; } break;

		case 0x24: { BIT(get_zerp()); pc++; } break;
		case 0xc5: { CMPM(get_zerp()); pc++; } break;
		case 0xe4: { CPXM(get_zerp()); pc++; } break;
		case 0xc4: { CPYM(get_zerp()); pc++; } break;

			//Zero Page Xy
		case 0xb5: { LDAM(get_zerx()); pc++; } break;
		case 0xb4: { LDYM(get_zerx()); pc++; } break;
		case 0xb6: { LDXM(get_zery()); pc++; } break;

		case 0x95: { STA(get_zerx()); pc++; } break;
		case 0x94: { STY(get_zerx()); pc++; } break;
		case 0x96: { STX(get_zery()); pc++; } break;

		case 0xf6: { INC(get_zerx()); pc++; } break;
		case 0xd6: { DEC(get_zerx()); pc++; } break;
		case 0x16: { ASLM(get_zerx()); pc++; } break;
		case 0x56: { LSRM(get_zerx()); pc++; } break;
		case 0x36: { ROLM(get_zerx()); pc++; } break;
		case 0x76: { RORM(get_zerx()); pc++; } break;

		case 0x75: { ADCM(get_zerx()); pc++; } break;
		case 0xf5: { SBCM(get_zerx()); pc++; } break;
		case 0x15: { ORAM(get_zerx()); pc++; } break;
		case 0x35: { ANDM(get_zerx()); pc++; } break;
		case 0x55: { EORM(get_zerx()); pc++; } break;

		case 0xd5: { CMPM(get_zerx()); pc++; } break;

			//Absolute
		case 0xad: { LDAM(get_abso()); pc += 2; } break;
		case 0xae: { LDXM(get_abso()); pc += 2; } break;
		case 0xac: { LDYM(get_abso()); pc += 2; } break;

		case 0x8d: { STA(get_abso()); pc += 2; } break;
		case 0x8e: { STX(get_abso()); pc += 2; } break;
		case 0x8c: { STY(get_abso()); pc += 2; } break;

		case 0xee: { INC(get_abso()); pc += 2; } break;
		case 0xce: { DEC(get_abso()); pc += 2; } break;
		case 0x0e: { ASLM(get_abso()); pc += 2; } break;
		case 0x4e: { LSRM(get_abso()); pc += 2; } break;
		case 0x2e: { ROLM(get_abso()); pc += 2; } break;
		case 0x6e: { RORM(get_abso()); pc += 2; } break;

		case 0x6d: { ADCM(get_abso()); pc += 2; } break;
		case 0xed: { SBCM(get_abso()); pc += 2; } break;
		case 0x0d: { ORAM(get_abso()); pc += 2; } break;
		case 0x2d: { ANDM(get_abso()); pc += 2; } break;
		case 0x4d: { EORM(get_abso()); pc += 2; } break;

		case 0xcd: { CMPM(get_abso()); pc += 2; } break;
		case 0x2c: { BIT(get_abso()); pc += 2; } break;
		case 0xec: { CPXM(get_abso()); pc += 2; } break;
		case 0xcc: { CPYM(get_abso()); pc += 2; } break;

			//Absolute XY
		case 0xbd: { LDAM(get_absx()); pc += 2; } break;
		case 0xb9: { LDAM(get_absy()); pc += 2; } break;
		case 0xbc: { LDYM(get_absx()); pc += 2; } break;
		case 0xbe: { LDXM(get_absy()); pc += 2; } break;

		case 0x9d: { STA(get_absx()); pc += 2; } break;
		case 0x99: { STA(get_absy()); pc += 2; } break;

		case 0xfe: { INC(get_absx()); pc += 2; } break;
		case 0xde: { DEC(get_absx()); pc += 2; } break;
		case 0x1e: { ASLM(get_absx()); pc += 2; } break;
		case 0x5e: { LSRM(get_absx()); pc += 2; } break;
		case 0x3e: { ROLM(get_absx()); pc += 2; } break;
		case 0x7e: { RORM(get_absx()); pc += 2; } break;

		case 0x7d: { ADCM(get_absx()); pc += 2; } break;
		case 0x79: { ADCM(get_absy()); pc += 2; } break;
		case 0xfd: { SBCM(get_absx()); pc += 2; } break;
		case 0xf9: { SBCM(get_absy()); pc += 2; } break;
		case 0x1d: { ORAM(get_absx()); pc += 2; } break;
		case 0x19: { ORAM(get_absy()); pc += 2; } break;
		case 0x3d: { ANDM(get_absx()); pc += 2; } break;
		case 0x39: { ANDM(get_absy()); pc += 2; } break;
		case 0x5d: { EORM(get_absx()); pc += 2; } break;
		case 0x59: { EORM(get_absy()); pc += 2; } break;

		case 0xdd: { CMPM(get_absx()); pc += 2; } break;
		case 0xd9: { CMPM(get_absy()); pc += 2; } break;

			//Indirect X
		case 0xa1: { LDAM(get_indx()); pc++; } break;

		case 0x81: { STA(get_indx()); pc++; } break;

		case 0xc1: { CMPM(get_indx()); pc++; } break;

		case 0x61: { ADCM(get_indx()); pc++; } break;
		case 0xe1: { SBCM(get_indx()); pc++; } break;
		case 0x01: { ORAM(get_indx()); pc++; } break;
		case 0x21: { ANDM(get_indx()); pc++; } break;
		case 0x41: { EORM(get_indx()); pc++; } break;

			//Indirect Y
		case 0xb1: { LDAM(get_indy()); pc++; } break;

		case 0x91: { STA(get_indy()); pc++; } break;

		case 0xd1: { CMPM(get_indy()); pc++; } break;

		case 0x11: { ORAM(get_indy()); pc++; } break;
		case 0xf1: { SBCM(get_indy()); pc++; } break;
		case 0x71: { ADCM(get_indy()); pc++; } break;
		case 0x31: { ANDM(get_indy()); pc++; } break;
		case 0x51: { EORM(get_indy()); pc++; } break;

			//Branches
		case 0x90: { BCC(get_rela()); } break;
		case 0x50: { BVC(get_rela()); } break;
		case 0xd0: { BNE(get_rela()); } break;
		case 0x30: { BMI(get_rela()); } break;
		case 0x10: { BPL(get_rela()); } break;
		case 0xf0: { BEQ(get_rela()); } break;
		case 0xb0: { BCS(get_rela()); } break;
		case 0x70: { BVS(get_rela()); } break;

			//Stack
		case 0x48: { PHA(); } break;
		case 0x08: { PHP(); } break;

		case 0x68: { PLA(); } break;
		case 0x28: { PLP(); } break;

		case 0x9a: { TXS(); } break;
		case 0xba: { TSX(); } break;

			//Jumps
		case 0x4c: { pc = get_abso(); } break;
		case 0x6c: { pc = get_indi(); } break;
		case 0x20: { JSR(get_abso()); } break;

			//Returns
		case 0x60: { RTS(); } break;
		case 0x40: { RTI(); } break;
		case 0x00: { BRK(); } break;
		}

		if (ppu_nmi)
		{
			NMI();
		}

		if (pagecrossed)
		{
			cpucycles++;
			totalcycles++;
			ppucycles += 3;
			pagecrossed = false;
		}

		cpucycles += cyclestable[op];
		totalcycles += cyclestable[op];
		ppucycles += cyclestable[op] * 3;
	}

	ppucycles -= CYCLES_PER_FRAME;
	//ppucycles -= CYCLES_PER_FRAME;
}

void set_cpu_status()
{
	int t = 0;
	t = fc ? 0x01 : 0x00;
	t |= fz ? 0x02 : 0x00;
	t |= fi ? 0x04 : 0x00;
	t |= fd ? 0x08 : 0x00;
	t |= fb ? 0x10 : 0x00;
	t |= fu ? 0x20 : 0x00;
	t |= fv ? 0x40 : 0x00;
	t |= fn ? 0x80 : 0x00;

	ps = t;
}

void get_flags(char* flags)
{
	sprintf(flags, " P:%c%c%c%c%c%c%c%c \n",
		ps & 0x80 ? 'N' : 'n',
		ps & 0x40 ? 'V' : 'v',
		ps & 0x20 ? 'U' : 'u',
		ps & 0x10 ? 'B' : 'b',
		ps & 0x08 ? 'D' : 'd',
		ps & 0x04 ? 'I' : 'i',
		ps & 0x02 ? 'Z' : 'z',
		ps & 0x01 ? 'C' : 'c'
	);
}

void BRK()
{
	//app_running = false;
}

void NMI()
{
	//pc++;
	ram[sp | 0x100] = pc >> 8;
	sp--;
	ram[sp | 0x100] = pc & 0xff;
	sp--;
	ram[sp | 0x100] = ps;
	sp--;
	pc = cpu_read_word(0xfffa);
	ppu_nmi = false;
}

void RTI()
{
	u8 hi, lo;
	PLP();
	sp++;
	lo = cpu_read(sp | 0x100);
	sp++;
	hi = cpu_read(sp | 0x100);
	pc = (hi << 8 | lo);
}

void RTS()
{
	u8 hi, lo;
	sp++;
	lo = cpu_read(sp | 0x100);
	sp++;
	hi = cpu_read(sp | 0x100);
	pc = (hi << 8 | lo) + 1;
}

void JSR(u16 addr)
{
	pc++;
	ram[sp | 0x100] = pc >> 8;
	sp--;
	ram[sp | 0x100] = pc & 0xff;
	sp--;
	pc = addr;
}

void TSX()
{
	x = sp;
	set_zero(x == 0);
	set_negative(x & 0x80);
}

void TXS()
{
	sp = x;
}

void PLP()
{
	sp++;
	ps = cpu_read(sp | 0x100);
	update_flags();
	//ps = (ps & 0xef);
}

void PLA()
{
	sp++;
	a = cpu_read(sp | 0x100);
	set_zero(a == 0);
	set_negative(a & 0x80);
}

void PHP()
{
	set_cpu_status();
	ram[sp | 0x100] = ps | 0x30;
	sp--;
	update_flags();
}

void PHA()
{
	ram[sp | 0x100] = a;
	sp--;
}

void BVS(u16 v)
{
	if (fv)
	{
		pc = v;
		cpucycles++;
		totalcycles++;
		ppucycles += 3;
	}
	else
	{
		pc++;
	}
}

void BCS(u16 v)
{
	if (fc)
	{
		pc = v;
		cpucycles++;
		totalcycles++;
		ppucycles += 3;
	}
	else
	{
		pc++;
	}
}

void BEQ(u16 v)
{
	if (fz)
	{
		pc = v;
		cpucycles++;
		totalcycles++;
		ppucycles += 3;
	}
	else
	{
		pc++;
	}
}

void BPL(u16 v)
{
	if (!fn)
	{
		pc = v;
		cpucycles++;
		totalcycles++;
		ppucycles += 3;
	}
	else
	{
		pc++;
	}
}

void BMI(u16 v)
{
	if (fn)
	{
		pc = v;
		cpucycles++;
		totalcycles++;
		ppucycles += 3;
	}
	else
	{
		pc++;
	}
}

void BNE(u16 v)
{
	if (!fz)
	{
		pc = v;
		cpucycles++;
		totalcycles++;
		ppucycles += 3;
	}
	else
	{
		pc++;
	}
}

void BVC(u16 v)
{
	if (!fv)
	{
		pc = v;
		cpucycles++;
		totalcycles++;
		ppucycles += 3;
	}
	else
	{
		pc++;
	}
}

void BCC(u16 v)
{
	if (!fc)
	{
		pc = v;
		cpucycles++;
		totalcycles++;
		ppucycles += 3;
	}
	else
	{
		pc++;
	}
}

void BIT(u16 addr)
{
	u8 b = cpu_read(addr);
	u8 t = a & b;
	set_zero(t == 0);
	set_negative(b & 0x80);
	set_overflow(b & 0x40);
}

void AND()
{
	a &= cpu_read(pc);
	set_zero(a == 0);
	set_negative(a & 0x80);
}

void ANDM(u16 addr)
{
	a &= cpu_read(addr);
	set_zero(a == 0);
	set_negative(a & 0x80);
}

void DEC(u16 addr)
{
	u8 b = cpu_read(addr) - 1;
	ram[addr] = b;
	set_zero(b == 0);
	set_negative(b & 0x80);
}

void INC(u16 addr)
{
	u8 b = cpu_read(addr) + 1;
	ram[addr] = b;
	set_zero(b == 0);
	set_negative(b & 0x80);
}

void STY(u16 addr)
{
	pagecrossed = false;
	cpu_write(addr, y);
}

void STX(u16 addr)
{
	pagecrossed = false;
	cpu_write(addr, x);
}

void STA(u16 addr)
{
	pagecrossed = false;
	cpu_write(addr, a);
}

void LDY()
{
	y = cpu_read(pc);
	set_zero(y == 0);
	set_negative(y & 0x80);
}

void LDYM(u16 addr)
{
	y = cpu_read(addr);
	set_zero(y == 0);
	set_negative(y & 0x80);
}

void LDX()
{
	x = cpu_read(pc);
	set_zero(x == 0);
	set_negative(x & 0x80);
}

void LDXM(u16 addr)
{
	x = cpu_read(addr);
	set_zero(x == 0);
	set_negative(x & 0x80);
}

void CPY()
{
	u8 v = cpu_read(pc);
	int t = y - v;
	set_carry(y >= v);
	set_zero(y == v);
	set_negative(t & 0x80);
}

void CPYM(u16 addr)
{
	u8 v = cpu_read(addr);
	int t = y - v;
	set_carry(y >= v);
	set_zero(y == v);
	set_negative(t & 0x80);
}

void CPX()
{
	u8 v = cpu_read(pc);
	int t = x - v;
	set_carry(x >= v);
	set_zero(x == v);
	set_negative(t & 0x80);
}

void CPXM(u16 addr)
{
	u8 v = cpu_read(addr);
	int t = x - v;
	set_carry(x >= v);
	set_zero(x == v);
	set_negative(t & 0x80);
}

void CMP()
{
	u8 v = cpu_read(pc);
	int t = a - v;
	set_carry(a >= v);
	set_zero(a == v);
	set_negative(t & 0x80);
}

void CMPM(u16 addr)
{
	u8 v = cpu_read(addr);
	int t = a - v;
	set_carry(a >= v);
	set_zero(a == v);
	set_negative(t & 0x80);
}

void EOR()
{
	a ^= cpu_read(pc);
	set_zero(a == 0);
	set_negative(a & 0x80);
}

void EORM(u16 addr)
{
	a ^= cpu_read(addr);
	set_zero(a == 0);
	set_negative(a & 0x80);
}

void ORA()
{
	a |= cpu_read(pc);
	set_zero(a == 0);
	set_negative(a & 0x80);
}

void ORAM(u16 addr)
{
	a |= cpu_read(addr);
	set_zero(a == 0);
	set_negative(a & 0x80);
}

void SBC()
{
	u8 v = cpu_read(pc);
	u16 r = a + ~v + (fc ? 1 : 0);
	set_zero((r & 0xff) == 0);
	set_negative(r & 0x80);
	set_overflow((a ^ v) & (a ^ r) & 0x80);
	set_carry((r & 0xff00) == 0);
	a = r;
}

void SBCM(u16 addr)
{
	u8 v = cpu_read(addr);
	u16 r = a + ~v + (fc ? 1 : 0);
	set_zero((r & 0xff) == 0);
	set_negative(r & 0x80);
	set_overflow((a ^ v) & (a ^ r) & 0x80);
	set_carry((r & 0xff00) == 0);
	a = r;
}

void ADC()
{
	u8 v = cpu_read(pc);
	u16 r = a + v + (fc ? 1 : 0);
	set_zero((r & 0xff) == 0);
	set_negative(r & 0x80);
	set_overflow((~(a ^ v) & (a ^ r) & 0x80));
	set_carry(r > 255);
	a = r;
}

void ADCM(u16 addr)
{
	u8 v = cpu_read(addr);
	u16 r = a + v + (fc ? 1 : 0);
	set_zero((r & 0xff) == 0);
	set_negative(r & 0x80);
	set_overflow(~(a ^ v) & (a ^ r) & 0x80);
	set_carry(r > 255);
	a = r;
}

void LDA()
{
	a = cpu_read(pc);;
	set_zero(a == 0);
	set_negative(a & 0x80);
}

void LDAM(u16 addr)
{
	a = cpu_read(addr);
	set_zero(a == 0);
	set_negative(a & 0x80);
}

void update_flags()
{
	fc = (ps & 0x01) ? true : false;
	fz = (ps & 0x02) ? true : false;
	fi = (ps & 0x04) ? true : false;
	fd = (ps & 0x08) ? true : false;
	fb = (ps & 0x10) ? true : false;
	fu = (ps & 0x20) ? true : false;
	fv = (ps & 0x40) ? true : false;
	fn = (ps & 0x80) ? true : false;
}

void DEY()
{
	y--;
	set_zero(y == 0);
	set_negative(y & 0x80);
}

void DEX()
{
	x--;
	set_zero(x == 0);
	set_negative(x & 0x80);
}

void INY()
{
	y++;
	set_zero(y == 0);
	set_negative(y & 0x80);
}

void INX()
{
	x++;
	set_zero(x == 0);
	set_negative(x & 0x80);
}

void TAY()
{
	y = a;
	set_zero(y == 0);
	set_negative(y & 0x80);
}

void TAX()
{
	x = a;
	set_zero(x == 0);
	set_negative(x & 0x80);
}

void TYA()
{
	a = y;
	set_zero(a == 0);
	set_negative(a & 0x80);
}

void TXA()
{
	a = x;
	set_zero(a == 0);
	set_negative(a & 0x80);
}

void ROL()
{
	bool bit7 = false;
	bit7 = a & (1 << 7) ? true : false;
	a <<= 1 & 0xff;
	if (fc)
		a |= (1 << 0);
	set_zero(a == 0);
	set_negative(a & 0x80);
	set_carry(bit7);
}

void ROLM(u16 addr)
{
	bool bit7 = false;
	u8 r = cpu_read(addr);
	bit7 = r & (1 << 7) ? true : false;
	r <<= 1;
	if (fc)
		r |= (1 << 0);
	set_carry(bit7);
	ram[addr] = r;
	set_zero(r == 0);
	set_negative(r & 0x80);
}

void ASL()
{
	set_carry(a & (1 << 7));
	a = (a << 1) & 0xfe;
	set_zero(a == 0);
	set_negative(a & 0x80);
}

void ASLM(u16 addr)
{
	u8 r = cpu_read(addr);
	set_carry(a & (1 << 7));
	r = (a << 1) & 0xfe;
	ram[addr] = r;
	set_zero(r);
	set_negative(r);
}

void ROR()
{
	bool bit0 = false;
	bit0 = a & (1 << 0) ? true : false;
	a >>= 1 & 0xff;
	if (fc)
		a |= (1 << 7);
	set_zero(a == 0);
	set_negative(a & 0x80);
	set_carry(bit0);
}

void RORM(u16 addr)
{
	bool bit0 = false;
	u8 r = cpu_read(addr);
	bit0 = r & (1 << 0) ? true : false;
	r >>= 1;
	if (fc)
		r |= (1 << 7);
	set_carry(bit0);
	ram[addr] = r;
	set_zero(r);
	set_negative(r);
}

void LSR()
{
	set_carry(a & (1 << 0));
	a = ((a >> 1) & 0x7f);
	set_zero(a == 0);
	set_negative(a & 0x80);
}

void LSRM(u16 addr)
{
	u8 r = cpu_read(addr);
	set_carry(r & (1 << 0));
	r = ((r >> 1) & 0x7f);
	ram[addr] = r;
	set_zero(r);
	set_negative(r);
}

void set_carry(u8 v)
{
	fc = v;

	if (fc)
	{
		ps |= 0x01;
	}
	else
	{
		ps &= ~0x01;
	}
}

void set_zero(u8 v)
{
	fz = v;

	if (fz)
	{
		ps |= 0x02;
	}
	else
	{
		ps &= ~0x02;
	}
}

void set_negative(u8 v)
{
	fn = v;

	if (fn)
	{
		ps |= 0x80;
	}
	else
	{
		ps &= ~0x80;
	}
}

void set_overflow(u8 v)
{
	fv = v;

	if (fv)
	{
		ps |= 0x40;
	}
	else
	{
		ps &= ~0x40;
	}
}

u8 get_imme()
{
	return cpu_read(pc);
}

u8 get_zerp()
{
	return cpu_read(pc);
}

u8 get_zerx()
{
	return (cpu_read(pc) + x);
}

u8 get_zery()
{
	return (cpu_read(pc) + y);
}

u16 get_abso()
{
	return cpu_read_word(pc);
}

u16 get_absx()
{
	u16 oldaddr = cpu_read_word(pc);
	u32 newaddr = oldaddr + x;
	pagecrossed = (newaddr & 0xff00) != (oldaddr & 0xff00) ? true : false;
	return newaddr & 0xffff;
}

u16 get_absy()
{
	u16 oldaddr = cpu_read_word(pc);
	u32 newaddr = oldaddr + y;
	pagecrossed = (newaddr & 0xff00) != (oldaddr & 0xff00) ? true : false;
	return newaddr & 0xffff;
}

u16 get_indx()
{
	u8 b1 = cpu_read(pc);
	u8 lo = cpu_read(b1 + x & 0xff);
	u8 hi = cpu_read(b1 + 1 + x & 0xff);
	return hi << 8 | lo;
}

u16 get_indy()
{
	u8 b1 = cpu_read(pc);
	u8 lo = cpu_read(b1 & 0xff);
	u8 hi = cpu_read(b1 + 1 & 0xff);
	u16 oldaddr = (hi << 8 | lo);
	u32 newaddr = oldaddr + y;
	pagecrossed = (newaddr & 0xff00) != (oldaddr & 0xff00) ? true : false;
	return newaddr & 0xffff;
}

u16 get_indi()
{
	u16 addr = cpu_read_word(pc);

	if ((addr & 0xff) == 0xff)
	{
		return ++addr;
	}
	else
	{
		return cpu_read_word(addr);
	}
}

u16 get_rela()
{
	u8 b1 = cpu_read(pc);
	return pc + (s8)(b1 + 1);
}