#include "../include/ppu.h"
#include "../include/mapper.h"

void ppu_init()
{
	scanline = 0;

	cycle = 0;

	FILE* fp = fopen("palettes/ASQ_realityA.pal", "rb");

	if (fp == NULL)
	{
		exit(1);
	}

	u8 palbuffer[64 * 3];
	if (fread(palbuffer, sizeof(u8), 64 * 3, fp) == 0)
	{
		exit(1);
	}

	fclose(fp);

	for (int i = 0; i < sizeof(palbuffer); i += 3)
	{
		palettes[i / 3] = palbuffer[i] | palbuffer[i + 1] << 8 | palbuffer[i + 2] << 16 | 0xff000000;
	}

	nametableaddr = 0x2000;

	ppu_dummy2007 = 0;
	screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, APP_WIDTH, APP_HEIGHT);
}

void ppu_draw_frame()
{
	//printf("%i\n", -scroll_x);

	u16 sx = scroll_x + ((ppuctrl & 1) ? 256 : 0);
	SDL_Rect rect = { -sx, -8 * APP_SCALE, 208 * APP_SCALE, 208 * APP_SCALE };
	SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
	SDL_RenderFillRect(renderer, &rect);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_UpdateTexture(screen, NULL, gfxdata, 256 * sizeof(unsigned char) * 4);
	SDL_RenderCopy(renderer, screen, NULL, NULL);

	SDL_RenderPresent(renderer);
}

void ppu_set_nmi()
{
	if (ppuctrl & 0x80)
	{
		ppu_nmi = true;
	}
}

void write_ppu_ctrl(u8 v)
{
	//ppuctrl = v;
	//nametableaddr = 0x2000 ^ ((ppuctrl & 3) * 0x400);

	//t = (t & 0xf3ff) | (v & 0x3);
}

void ppu_set_vblank()
{
	ppustatus |= 0x80;
}

void ppu_clear_vblank()
{
	ppustatus &= 0x7f;
}

void ppu_set_sprite0()
{
	ppustatus |= 0x40;
}

void ppu_clear_sprite0()
{
	ppustatus &= 0xbf;
}

void ppu_render()
{
	if (scanline < 240)
	{
		if (scanline == 0)
		{
			ppu_set_sprite0();
		}

		if (ppumask & 0x10)
		{
			ppu_render_sprites(0);
		}

		if (ppumask & 0x08)
		{
			ppu_render_background(0x2000);
			//ppu_render_background(0x2000 + (mirrornametable) * 0x400);
		}

		if (ppumask & 0x10)
		{
			ppu_render_sprites(0);
		}
	}
	else if (scanline == 241)
	{
		if (cycle == 1)
		{
			ppu_set_vblank();
			ppu_set_nmi();
			ppu_clear_sprite0();
			cycle = 0;
			ppu_draw_frame();
		}
	}
	else if (scanline == 261)
	{
		ppu_clear_vblank();
		ppu_clear_sprite0();
		cycle = 1;
	}
	else if (scanline == 262)
	{

		scanline = -1;
		ppu_nmi = false;
		//memset(gfxdata, 0, sizeof(gfxdata));
	}

	scanline++;
}

void increase_x()
{
	if ((v & 0x1f) == 0x1f)
	{
		v &= ~0x1f;
		v ^= 0x400;
	}
	else
	{
		v++;
	}
}

void ppu_render_background(u16 nameaddr)
{
	if (ppuctrl & 1)
	{
		int yu = 0;
	}

	//ppuctrl = 0x11;
	//u16 sx = scroll_x + ((ppuctrl & 1) ? 256 : 0);
	//int nametableaddr = 0x2000 ^ ((ppuctrl & 1) ? 0x400 : 0);
	u8 sx = scroll_x;// +((ppuctrl & 1) ? 256 : 0);
	//int nametableaddr = 0x2000 ^ ((ppuctrl & 1) ? 0x400 : 0);
	int patternaddr = ppuctrl & 0x10 ? 0x1000 : 0x0000;
	int paladdr = ppuctrl & 0x10 ? 0x3f10 : 0x3f00;
	int y = scanline;

	u8 byte1, byte2;

	//vram[0x2000] = 0x16;

	for (int x = 0; x < 256; x++)
	{
		int addr = 0x2000 + ((ppuctrl & 1) ? 0x800 : 0) + (y / 8 * 32) + x / 8;
		int tileid = vram[addr];

		if (addr == 0x2020)
		{
			int yu = 0;
		}

		int bit2 = get_attribute_index(addr & 0x1f, ((addr & 0x3e0) >> 5), vram[0x2000 + 0x3c0 + (y / 32) * 8 + (x / 32)]);

		byte1 = vram[patternaddr + tileid * 16 + (y % 8) + 0];
		byte2 = vram[patternaddr + tileid * 16 + (y % 8) + 8];

		int bit0 = byte1 >> (7 - (x % 8)) & 1;
		int bit1 = byte2 >> (7 - (x % 8)) & 1;

		int colorindex = bit2 * 4 + (bit0 | bit1 * 2);

		gfxdata[y * 256 + x - sx] = palettes[vram[0x3f00 + colorindex]];
	}
}

void ppu_render_sprites(u8 frontback)
{
	int oamaddr = 0x0100 * ppuoamdma;
	int nametableaddr = 0x2000 | ppuctrl & 3 * 0x400;
	int patternaddr = ppuctrl & 0x10 ? 0x0000 : 0x1000;
	int paladdr = ppuctrl & 0x10 ? 0x3f10 : 0x3f00;

	u8 bytes1[8], bytes2[8];
	u8 y, tileid, att, x, i;

	int sprcount = 0;
	for (int j = 64; j > 0; j--)
	{
		i = j % 64;

		if (ppuoamdma == 0)
		{
			y = oammem[i * 4 + 0] + 1 & 0xff;
			tileid = oammem[i * 4 + 1];
			att = oammem[i * 4 + 2];
			x = oammem[i * 4 + 3] & 0xff;
		}
		else
		{
			y = ram[oamaddr | i * 4 + 0] + 1;
			tileid = ram[oamaddr | i * 4 + 1];
			att = ram[oamaddr | i * 4 + 2];
			x = ram[oamaddr | i * 4 + 3];
		}

		if (y >= 0xe9 || x >= 0xf9)
		{
			continue;
		}

		if (att & frontback)
		{
			continue;
		}

		bool flipH = att & 0x40;
		bool flipV = att & 0x80;

		memcpy(bytes1, vram + patternaddr + tileid * 16 + 0, 8);
		memcpy(bytes2, vram + patternaddr + tileid * 16 + 8, 8);

		for (int r = 0; r < 8; r++)
		{
			for (int cl = 0; cl < 8; cl++)
			{
				int col = 7 - cl;
				int row = r;

				if (flipH & flipV)
				{
					col = cl;
					row = 7 - r;
				}
				else if (flipV)
				{
					row = 7 - r;
				}
				else if (flipH)
				{
					col = cl;
				}

				int bit0 = bytes1[row] & 1 ? 1 : 0;
				int bit1 = bytes2[row] & 1 ? 2 : 0;

				bytes1[row] >>= 1;
				bytes2[row] >>= 1;

				int palindex = bit0 | bit1;

				int colorindex = palindex + (att & 3) * 4;

				if (palindex != 0)
				{
					gfxdata[256 * (y + row) + x + col] = palettes[vram[paladdr + colorindex]];
				}
			}
		}
	}
}

int get_attribute_index(int x, int y, int attrib)
{
	//get the right attribute
	if (y & 2)
	{
		if (x & 2)
		{
			return (attrib & 0xc0) >> 6;
		}
		else
		{
			return (attrib & 0x30) >> 4;
		}
	}
	else
	{
		if (x & 2)
		{
			return (attrib & 0x0c) >> 2;
		}
		else
		{
			return (attrib & 0x03) >> 0;
		}
	}
}

void ppu_clean()
{
	free(palettes);
}