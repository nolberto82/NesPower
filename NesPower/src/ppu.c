#include "../include/ppu.h"
#include "../include/mapper.h"

void ppu_init()
{
	scanline = 0;

	cycle = 0;

	FILE* fp = fopen("../../../../palettes/ASQ_realityA.pal", "rb");

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

	scale_x = 1;
	scale_y = 1;
	SDL_RenderSetScale(renderer, scale_x, scale_y);

	if (screen == NULL)
		exit(1);
}

void ppu_draw_frame()
{
	SDL_Rect rect = { 0, -8 * APP_SCALE, 256, 240 };
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
			ppu_render_background();
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
		ppuctrl &= 0xfe;
		cycle = 1;
	}
	else if (scanline == 262)
	{

		scanline = -1;
		ppu_nmi = false;
		//memset(gfxdata, 0, sizeof(gfxdata));
		//return;
	}

	scanline++;
}

void increase_x()
{
	if ((ppu_v & 0x1f) == 0x1f)
	{
		ppu_v &= ~0x1f;
		ppu_v ^= 0x400;
	}
	else
	{
		ppu_v++;
	}
}

void ppu_render_background()
{
	if (scroll_x == 0)
	{
		int nm = 0;
	}
	else if (scroll_x > 0)
	{
		int nm = 1;
	}

	int patternaddr = ppuctrl & 0x10 ? 0x1000 : 0x0000;
	int paladdr = ppuctrl & 0x10 ? 0x3f10 : 0x3f00;
	int y = scanline / 8 + 1;

	u8 byte1, byte2;

	u8 sx = scroll_x;
	//u8 xp = ppu_v & 0x1f;

	int xMin = scroll_x / 8;
	int xMax = (scroll_x + 256) / 8;

	//ram[0x86] = 0x80;
	//sx = 2;

	for (int x = xMin; x <= xMax; x++)
	{
		int addr = 0;
		int natx = 0;

		if (x < 32)
		{
			addr = 0x2000 + 32 * y + x;
		}
		else if (x < 64)
		{
			addr = 0x2400 + 32 * y + (x - 32);
			natx = 32;
		}
		else
		{
			addr = 0x2800 + 32 * y + (x - 64);
			natx = 64;
		}

		addr ^= ppuctrl & 1 ? 0x400 : 0;

		int offx = x * 8 - sx;
		int offy = y * 8;

		int baseaddr = addr & 0x2c00;

		int tileid = vram[addr];

		int bit2 = get_attribute_index(addr & 0x1f, ((addr & 0x3e0) >> 5), vram[baseaddr + 0x3c0 + (y / 4) * 8 + ((x - natx) / 4)]);

		for (int row = 0; row < 8; row++)
		{
			byte1 = vram[patternaddr + tileid * 16 + row + 0];
			byte2 = vram[patternaddr + tileid * 16 + row + 8];

			for (int col = 0; col < 8; col++)
			{
				int xp = offx + (7 - col);
				int yp = offy + row;

				if (xp > 255)
					continue;

				int bit0 = byte1 & 1 ? 1 : 0;
				int bit1 = byte2 & 1 ? 1 : 0;

				byte1 >>= 1;
				byte2 >>= 1;

				int colorindex = bit2 * 4 + (bit0 | bit1 * 2);

				gfxdata[yp * 256 + xp] = palettes[vram[0x3f00 + colorindex]];
			}
		}

		//ppu_draw_frame();
	}
}

void ppu_render_nametables()
{
	int patternaddr = ppuctrl & 0x10 ? 0x1000 : 0x0000;
	int paladdr = ppuctrl & 0x10 ? 0x3f10 : 0x3f00;
	int baseaddr = 0x2000;

	for (int addr = 0; addr < 0x3c0; addr++)
	{
		u8 sx = addr & 0x1f;
		u16 tileid = vram[baseaddr + addr];
	}

}

void ppu_render_sprites(u8 frontback)
{
	int oamaddr = 0x0100 * ppuoamdma;
	int nametableaddr = 0x2000 | ppuctrl & 3 * 0x400;
	int patternaddr = ppuctrl & 0x10 ? 0x0000 : 0x1000;
	int paladdr = ppuctrl & 0x10 ? 0x3f10 : 0x3f00;

	u8 byte1, byte2;
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

		//memcpy(bytes1, vram + patternaddr + tileid * 16 + 0, 8);
		//memcpy(bytes2, vram + patternaddr + tileid * 16 + 8, 8);

		for (int r = 0; r < 8; r++)
		{
			byte1 = vram[patternaddr + tileid * 16 + r + 0];
			byte2 = vram[patternaddr + tileid * 16 + r + 8];

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

				int bit0 = byte1 & 1 ? 1 : 0;
				int bit1 = byte2 & 1 ? 1 : 0;

				byte1 >>= 1;
				byte2 >>= 1;

				int palindex = bit0 | bit1 * 2;

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