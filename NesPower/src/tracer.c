#include "../include/tracer.h"
#include "../include/mapper.h"

enum addrmode { IMPL, ACCU, IMME, ZERP, ZERX, ZERY, ABSO, ABSX, ABSY, INDX, INDY, INDI, RELA, ERRO };

const enum addrmode modes[256] =
{
	IMME, INDX, ERRO, INDX, ZERP, ZERP, ZERP, ZERP, IMPL, IMME, ACCU, IMME, ABSO, ABSO, ABSO, ABSO,
	RELA, INDY, ERRO, INDY, ZERX, ZERX, ZERX, ZERX, IMPL, ABSY, IMPL, ABSY, ABSX, ABSX, ABSX, ABSX,
	ABSO, INDX, ERRO, INDX, ZERP, ZERP, ZERP, ZERP, IMPL, IMME, ACCU, IMME, ABSO, ABSO, ABSO, ABSO,
	RELA, INDY, ERRO, INDY, ZERX, ZERX, ZERX, ZERX, IMPL, ABSY, IMPL, ABSY, ABSX, ABSX, ABSX, ABSX,
	IMPL, INDX, ERRO, INDX, ZERP, ZERP, ZERP, ZERP, IMPL, IMME, ACCU, IMME, ABSO, ABSO, ABSO, ABSO,
	RELA, INDY, ERRO, INDY, ZERX, ZERX, ZERX, ZERX, IMPL, ABSY, IMPL, ABSY, ABSX, ABSX, ABSX, ABSX,
	IMPL, INDX, ERRO, INDX, ZERP, ZERP, ZERP, ZERP, IMPL, IMME, ACCU, IMME, INDI, ABSO, ABSO, ABSO,
	RELA, INDY, ERRO, INDY, ZERX, ZERX, ZERX, ZERX, IMPL, ABSY, IMPL, ABSY, ABSX, ABSX, ABSX, ABSX,
	IMME, INDX, IMME, INDX, ZERP, ZERP, ZERP, ZERP, IMPL, IMME, IMPL, IMME, ABSO, ABSO, ABSO, ABSO,
	RELA, INDY, ERRO, INDY, ZERX, ZERX, ZERY, ZERY, IMPL, ABSY, IMPL, ABSY, ABSX, ABSX, ABSY, ABSY,
	IMME, INDX, IMME, INDX, ZERP, ZERP, ZERP, ZERP, IMPL, IMME, IMPL, IMME, ABSO, ABSO, ABSO, ABSO,
	RELA, INDY, ERRO, INDY, ZERX, ZERX, ZERY, ZERY, IMPL, ABSY, IMPL, ABSY, ABSX, ABSX, ABSY, ABSY,
	IMME, INDX, IMME, INDX, ZERP, ZERP, ZERP, ZERP, IMPL, IMME, IMPL, IMME, ABSO, ABSO, ABSO, ABSO,
	RELA, INDY, ERRO, INDY, ZERX, ZERX, ZERX, ZERX, IMPL, ABSY, IMPL, ABSY, ABSX, ABSX, ABSX, ABSX,
	IMME, INDX, IMME, INDX, ZERP, ZERP, ZERP, ZERP, IMPL, IMME, IMPL, IMME, ABSO, ABSO, ABSO, ABSO,
	RELA, INDY, ERRO, INDY, ZERX, ZERX, ZERX, ZERX, IMPL, ABSY, IMPL, ABSY, ABSX, ABSX, ABSX, ABSX
};

const char opnames[256][3] =
{
	"BRK", "ORA", "ERR", "ERR", "ERR", "ORA", "ASL", "ERR", "PHP", "ORA", "ASL", "ERR", "ERR", "ORA", "ASL", "ERR",
	"BPL", "ORA", "ERR", "ERR", "ERR", "ORA", "ASL", "ERR", "CLC", "ORA", "ERR", "ERR", "ERR", "ORA", "ASL", "ERR",
	"JSR", "AND", "ERR", "ERR", "BIT", "AND", "ROL", "ERR", "PLP", "AND", "ROL", "ERR", "BIT", "AND", "ROL", "ERR",
	"BMI", "AND", "ERR", "ERR", "ERR", "AND", "ROL", "ERR", "SEC", "AND", "ERR", "ERR", "ERR", "AND", "ROL", "ERR",
	"RTI", "EOR", "ERR", "ERR", "ERR", "EOR", "LSR", "ERR", "PHA", "EOR", "LSR", "ERR", "JMP", "EOR", "LSR", "ERR",
	"BVC", "EOR", "ERR", "ERR", "ERR", "EOR", "LSR", "ERR", "CLI", "EOR", "ERR", "ERR", "ERR", "EOR", "LSR", "ERR",
	"RTS", "ADC", "ERR", "ERR", "ERR", "ADC", "ROR", "ERR", "PLA", "ADC", "ROR", "ERR", "JMP", "ADC", "ROR", "ERR",
	"BVS", "ADC", "ERR", "ERR", "ERR", "ADC", "ROR", "ERR", "SEI", "ADC", "ERR", "ERR", "ERR", "ADC", "ROR", "ERR",
	"ERR", "STA", "ERR", "ERR", "STY", "STA", "STX", "ERR", "DEY", "ERR", "TXA", "ERR", "STY", "STA", "STX", "ERR",
	"BCC", "STA", "ERR", "ERR", "STY", "STA", "STX", "ERR", "TYA", "STA", "TXS", "ERR", "ERR", "STA", "ERR", "ERR",
	"LDY", "LDA", "LDX", "ERR", "LDY", "LDA", "LDX", "ERR", "TAY", "LDA", "TAX", "ERR", "LDY", "LDA", "LDX", "ERR",
	"BCS", "LDA", "ERR", "ERR", "LDY", "LDA", "LDX", "ERR", "CLV", "LDA", "TSX", "ERR", "LDY", "LDA", "LDX", "ERR",
	"CPY", "CMP", "ERR", "ERR", "CPY", "CMP", "DEC", "ERR", "INY", "CMP", "DEX", "ERR", "CPY", "CMP", "DEC", "ERR",
	"BNE", "CMP", "ERR", "ERR", "ERR", "CMP", "DEC", "ERR", "CLD", "CMP", "ERR", "ERR", "ERR", "CMP", "DEC", "ERR",
	"CPX", "SBC", "ERR", "ERR", "CPX", "SBC", "INC", "ERR", "INX", "SBC", "NOP", "SBC", "CPX", "SBC", "INC", "ERR",
	"BEQ", "SBC", "ERR", "ERR", "ERR", "SBC", "INC", "ERR", "SED", "SBC", "ERR", "ERR", "ERR", "SBC", "INC", "ERR"
};

void trace_disasm(u16 pc, u8 op, u8 x, u8 y, u8 sp, int cycles, char* strtrace)
{
	const char opstr[4] = { opnames[op][0],opnames[op][1],opnames[op][2],'\0' };
	char strcyc[64];
	char strpc[64];
	char strins[64];
	u16 addr, addr2;
	u8 b1 = cpu_read_debug(pc + 1);
	u8 b2 = cpu_read_debug(pc + 2);
	u8 v;
	u8 hi, lo;

	sprintf(strcyc, "c%d", cycles);

	switch (modes[op])
	{
	case IMPL:
	case ACCU:
		sprintf(strpc, "%04X:%02X", pc, op);
		if (op == 0x60)
		{
			addr = cpu_read_word(sp | 0x100 + 1) - 2;
			addr = cpu_read_word(addr + 1);
			sprintf(strins, "%s (from $%04X) --------------------------- ", opstr, addr);
		}
		else
		{
			sprintf(strins, "%s", opstr);
		}
		break;
	case IMME:
		sprintf(strpc, "%04X:%02X %02X", pc, op, b1);
		sprintf(strins, "%s #$%02X", opstr, b1);
		break;
	case ZERP:
		v = cpu_read_debug(b1);
		sprintf(strpc, "%04X:%02X %02X", pc, op, b1);
		sprintf(strins, "%s $%04X = #$%02X", opstr, b1, v);
		break;
	case ZERX:
		addr = b1 + x;
		v = cpu_read_debug(addr);
		sprintf(strpc, "%04X:%02X %02X", pc, op, b1);
		sprintf(strins, "%s $%02X,X @ $%04X = #$%02X", opstr, b1, addr, v);
		break;
	case ZERY:
		addr = b1 + y;
		v = cpu_read_debug(addr);
		sprintf(strpc, "%04X:%02X %02X", pc, op, b1);
		sprintf(strins, "%s $%04X,Y @ $%04X = #$%02X", opstr, b1, addr, v);
		break;
	case ABSO:
		addr = (b2 << 8) | b1;
		v = cpu_read_debug(addr);

		if (op != 0x4c && op != 0x20)
		{
			sprintf(strpc, "%04X:%02X %02X %02X", pc, op, b1, b2);
			sprintf(strins, "%s $%04X = #$%02X", opstr, addr, v);
		}
		else
		{
			sprintf(strpc, "%04X:%02X %02X %02X", pc, op, b1, b2);
			sprintf(strins, "%s $%04X", opstr, addr);
		}
		break;
	case ABSX:
		addr = b2 << 8 | b1;
		v = cpu_read_debug(addr + x & 0xffff);
		sprintf(strpc, "%04X:%02X %02X %02X", pc, op, b1, b2);
		sprintf(strins, "%s $%04X,X @ $%04X = #$%02X", opstr, addr, addr + x, v);
		break;
	case ABSY:
		addr = b2 << 8 | b1;
		v = cpu_read_debug(addr + y & 0xffff);
		sprintf(strpc, "%04X:%02X %02X %02X", pc, op, b1, b2);
		sprintf(strins, "%s $%04X,Y @ $%04X = #$%02X", opstr, addr, addr + y & 0xffff, v);
		break;
	case INDX:
		lo = cpu_read_debug(b1 + x & 0xff);
		hi = cpu_read_debug(b1 + x + 1 & 0xff);
		addr = hi << 8 | lo;
		v = cpu_read_debug(addr);
		sprintf(strpc, "%04X:%02X %02X", pc, op, b1);
		sprintf(strins, "%s ($%02X,X) @ $%04X = #$%02X", opstr, b1, addr, v);
		break;
	case INDY:
		lo = cpu_read_debug(b1 & 0xff);
		hi = cpu_read_debug(b1 + 1 & 0xff);
		addr = (hi << 8 | lo) + y & 0xffff;
		v = cpu_read_debug(addr);
		sprintf(strpc, "%04X:%02X %02X", pc, op, b1);
		sprintf(strins, "%s ($%02X),Y @ $%04X = #$%02X", opstr, b1, addr, v);
		break;
	case INDI:
		addr = b2 << 8 | b1;

		if (b1 == 0xff)
		{
			addr2 = addr + 1;
		}
		else
		{
			hi = cpu_read_debug(addr + 1);
			lo = cpu_read_debug(addr);
			addr2 = hi << 8 | lo;
		}

		sprintf(strpc, "%04X:%02X %02X %02X", pc, op, b1, b2);
		sprintf(strins, "%s ($%04X) = $%04X", opstr, addr, addr2);
		break;
	case RELA:
		addr = pc + (u8)b1 + 2;
		sprintf(strpc, "%04X:%02X %02X", pc, op, b1);
		sprintf(strins, "%s $%02X", opstr, addr);
		break;
	default:
		break;
	}

	sprintf(strtrace, "%-12s $%-14s %-45s", strcyc, strpc, strins);
}