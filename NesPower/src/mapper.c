#include "../include/mapper.h"
#include "../include/ppu.h"
#include "../include/controls.h"
#include "direct.h"
//#include "../main.h"

bool load_rom(char* filename)
{
	char cd[FILENAME_MAX];
	_getcwd(cd, sizeof(cd));
	printf("path: %s", cd);

	if (filename == NULL)
		return 0;

	FILE* fp = fopen(filename, "rb");

	if (fp == NULL)
		return false;

	fseek(fp, 0, SEEK_END);
	int fsize = ftell(fp);
	rewind(fp);

	rom = malloc(fsize);

	if (!rom)
	{
		fclose(fp);
		return 0;
	}

	int romsize = fread(rom, sizeof(u8), fsize, fp);

	if (romsize == 0)
	{
		free(rom);
		fclose(fp);
		return 0;
	}

	fclose(fp);

	set_mapper();

	return true;
}

void set_mapper()
{
	int prgbanks = rom[4];
	int chrbanks = rom[5];
	int prgsize = prgbanks * 0x4000;
	int chrsize = chrbanks * 0x2000;
	int mappernum = rom[8] & 0x0f;
	mirrornametable = 0;

	if (rom[6] & 0x08)
	{
		mirrornametable = 2;
	}
	else if (rom[6] & 0x01)
	{
		mirrornametable = 1;
	}

	switch (mappernum)
	{
	case 0:
		if (prgbanks == 1)
		{
			memcpy(&ram[0xc000], rom + 0x10, prgsize);
			memcpy(&vram[0x0000], rom + 0x10 + 0x4000, chrsize);
		}
		else
		{
			memcpy(&ram[0x8000], rom + 0x10, prgsize);
			memcpy(&vram[0x0000], rom + 0x10 + prgsize, chrsize);
		}
		break;
	}
}

u16 cpu_read_word(u16 addr)
{
	u8 hi = cpu_read(addr + 1);
	u8 lo = cpu_read(addr);
	return hi << 8 | lo;
}

u8 cpu_read_debug(u16 addr)
{
	switch (addr)
	{
	case 0x2002:
		break;
	default:
		break;
	}

	return ram[addr];
}

u8 cpu_read(u16 addr)
{
	u8 val = 0;
	switch (addr)
	{
	case 0x2002:
		val = ppustatus;
		ppu_clear_vblank();
		ram[addr] = ppustatus;
		ppu_w = 0;
		return val;
	case 0x2007:
		if (ppu_v < 0x3f00)
		{
			val = ppu_dummy2007;
			ppu_dummy2007 = vram[ppu_v];
		}

		if (ppuctrl & 0x04)
		{
			ppu_v += 32;
		}
		else
		{
			ppu_v++;
		}
		return val;
	case 0x4016:
		return controls_read();
	default:
		break;
	}

	return ram[addr];
}

void cpu_write(u16 addr, u8 val)
{
	switch (addr)
	{
	case 0x2000:
		ppuctrl = val;

		//write_ppu_ctrl(val);
		ppu_t = (ppu_t & 0xc00) | val << 10;

		if (val & 0x10)
		{
			ppustatus |= val;
			ram[0x2002] |= val;
		}
		break;
	case 0x2001:
		ppumask |= val;
		break;
	case 0x2003:
		ppuoamaddr = val;
		break;
	case 0x2004:
		ppuoamdata = val;
		oammem[ppuoamaddr++] = val;
		break;
	case 0x2005:
		if (!ppu_w)
		{
			ppu_t = (ppu_t & 0x7fe0) | (val >> 3);
			ppu_x = val & 0x07;
			scroll_x = val;
		}
		else
		{
			ppu_t = (ppu_t & 0xc1f) | ((val & 0x07) << 12) | ((val & 0xF8) << 2);
			scroll_y = val;
		}

		ppu_w ^= 1;
		break;
	case 0x2006:
		if (!ppu_w)
		{
			ppu_t = (ppu_t & 0xff) | val << 8;
		}
		else
		{
			ppu_t = (ppu_t & 0xff00) | val;
			ppu_v = ppu_t;
		}

		ppu_w ^= 1;
		break;
	case 0x2007:
		ppu_write(ppu_v, val);

		if (ppuctrl & 0x04)
		{
			ppu_v += 32;
		}
		else
		{
			ppu_v++;
		}
		break;
	case 0x4014:
		ppuoamdma = val;
		break;
	case 0x4016:
		controls_write(val);
		break;
	}

	ram[addr] = val;
}

u8 ppu_read(u16 addr)
{
	if (addr >= 0x3f10)
	{
		return vram[addr - 16];
	}

	return vram[addr];
}

void ppu_read_8_bytes(u16 addr, u8* bytearr)
{
	return memcpy(bytearr, vram + addr, 8);
}

void ppu_write(u16 addr, u8 v)
{
	vram[addr] = v;
}

void mapper_clean()
{
	FILE* fpvram = fopen("vram.bin", "wb");
	fwrite(vram, 0x4000, 1, fpvram);
	fclose(fpvram);

	if (rom)
		free(rom);
}