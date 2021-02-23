#include "../include/ppu.h"
#include "../include/mapper.h"

u16 nametables[4] = { 0x2000, 0x2400, 0x2800, 0x2c00 };

void ppu_init()
{
	ppu_scanline = 0;

	ppu_dots = 0;

	ppu_cyc = 0;

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

void ppu_ctrl_write(u8 val) //0x2000
{
	ppuctrl = val;

	ppu_t = (ppu_t & ~0b00110000000000);
	ppu_t |= (val & 3) << 10;

	if (val & 0x10)
	{
		ppustatus |= val;
		ram[0x2002] |= val;
	}
}

void ppu_mask_write(u8 val)
{
	ppumask |= val;

	backgroundrender = ppumask & 0x08;
	spritesrender = ppumask & 0x10;
}

void ppu_scroll_write(u8 val) //0x2005
{
	if (!ppu_w)
	{
		ppu_t = (ppu_t & ~0b00011111) | (val >> 3) & 0b00011111;// (ppu_t & 0x7fe0) | (val >> 3);
		ppu_x = val & 0x07;
		scroll_x = val;
	}
	else
	{
		ppu_t = (ppu_t & ~0b111001111100000);
		ppu_t |= (val & 7) << 12 | (val & 0b11111000) << 2;//(ppu_t & 0xc1f) | ((val & 0x07) << 12) | ((val & 0xF8) << 2);
		scroll_y = val;
	}

	ppu_w ^= 1;
}

void ppu_addr_write(u8 val) //0x2006
{
	if (!ppu_w)
	{
		ppu_t = (ppu_t & ~0b1111111100000000) | (val & 0b00111111) << 8;// (ppu_t & 0xff) | val << 8;
	}
	else
	{
		ppu_t = (ppu_t & ~0b11111111);
		ppu_t |= val;// (ppu_t & 0xff00) | val;
		ppu_v = ppu_t;
	}

	ppu_w ^= 1;
}

void ppu_data_write(u8 val) //0x2007
{
	ppu_write(ppu_v, val);

	if (ppuctrl & 0x04)
		ppu_v += 32;
	else
		ppu_v++;
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

void ppu_step()
{
	//if (ppu_scanline > 0 && ppu_scanline < 240)
	//{

	//}
	//else if (ppu_scanline == 261)
	//{
	//	if (backgroundrender)
	//	{
	//		if (ppu_dots > 0 && ppu_dots < 256)
	//		{

	//		}
	//	}
	//}

	//if (ppu_dots < 341)
	//	ppu_dots++;
	//else
	//	ppu_dots = 0;

	if (ppu_scanline < 239)
	{
		//if (ppu_scanline == 0)
		//{
		if (spritesrender)
		{
			int oamaddr = 0x0100 * ppuoamdma;
			u8 y = ram[oamaddr + 0];
			u8 x = ram[oamaddr + 3];

			if ((y + 8) == ppu_scanline)
				ppu_set_sprite0();
		}

		//ppu_v = ppu_t;
	//}

		if (spritesrender)
		{
			//ppu_render_sprites(0x20);
		}

		if (backgroundrender)
		{
			ppu_render_background();
		}

		if (spritesrender)
		{
			ppu_render_sprites(0x20);
		}

	}
	else if (ppu_scanline == 241)
	{
		if (ppu_cyc == 0)
		{
			ppu_set_vblank();
			ppu_set_nmi();
			ppu_clear_sprite0();
			ppu_draw_frame();
			ppu_cyc++;
		}
	}
	else if (ppu_scanline == 261)
	{
		if (ppu_cyc == 1)
		{
			ppu_clear_vblank();
			ppu_clear_sprite0();
			ppuctrl &= 0xfc;
			ppu_cyc = 0;
		}
	}
	else if (ppu_scanline == 262)
	{
		ppu_cyc = 0;
		ppu_scanline = -1;
		//ppu_cyc = -1;
		ppu_nmi = false;
		//memset(gfxdata, 0, sizeof(gfxdata));
		//return;
	}

	ppu_scanline++;
	if (ppu_dots < 341)
		ppu_dots++;
	else
		ppu_dots = 0;
}

void ppu_increase_v()
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

void ppu_render_background_new()
{
	u16 patternaddr = ppuctrl & 0x10 ? 0x1000 : 0x0000;
	u16 paladdr = ppuctrl & 0x10 ? 0x3f10 : 0x3f00;
	u16 y = ppu_scanline;
	//ppu_x = 2;

	for (int x = 0; x < 256; x++)
	{
		if ((ppu_v & 0x400) == 0x400 && ppu_scanline == 0)
		{
			int yu = 0;
		}

		u16 addr_nam = 0x2000 | (ppu_v & 0xfff);
		u16 addr_att = 0x23c0 | (ppu_v & 0xc00) | ((ppu_v >> 4) & 0x38) | ((ppu_v >> 2) & 7);

		if (ppuctrl & 1)
			addr_nam ^= 0x400;

		u16 tileid = vram[addr_nam];

		u8 byte1 = vram[patternaddr + tileid * 16 + (y % 8) + 0];
		u8 byte2 = vram[patternaddr + tileid * 16 + (y % 8) + 8];

		u8 bit2 = get_attribute_index(addr_nam & 0x1f, ((addr_nam & 0x3e0) >> 5), vram[addr_att]);

		u8 bit0 = byte1 >> (7 - (x % 8)) & 1;
		u8 bit1 = byte2 >> (7 - (x % 8)) & 1;

		int colorindex = bit2 * 4 + (bit0 | bit1 * 2);

		gfxdata[y * 256 + x - scroll_x] = palettes[vram[0x3f00 + colorindex]];
		//ppu_draw_frame();

		if ((x + 1) % 8 == 0)
			ppu_increase_v();

		if (x == 254)
		{
			//int yp = (ppu_v & 0x03E0) >> 5;
			//ppu_v = (ppu_v & ~0x03E0) | (y / 8) << 5;
			if ((ppu_v & 0x7000) != 0x7000)
				ppu_v += 0x1000;                   // increment fine Y
			else
			{
				ppu_v &= ~0x7000;                  // fine Y = 0
				int yp = (ppu_v & 0x03E0) >> 5;       // let y = coarse Y
				if (yp == 29)
				{
					yp = 0;                          // coarse Y = 0
					ppu_v ^= 0x0800;
				}
				else if (yp == 31)
				{
					yp = 0;                       // coarse Y = 0, nametable not switched
				}
				else
				{
					yp++;                        // increment coarse Y
					ppu_v = (ppu_v & ~0x03E0) | (yp << 5);
				}
			}
		}
	}

	//ppu_v &= ~0x41f;
	//ppu_v |= ppu_t & 0x41f;
}

void ppu_render_background()
{

	int patternaddr = ppuctrl & 0x10 ? 0x1000 : 0x0000;
	int paladdr = ppuctrl & 0x10 ? 0x3f10 : 0x3f00;
	int left8 = ppuctrl & 0x02 ? 1 : 0;
	int y = (ppu_scanline / 8);// +scroll_y;

	u8 byte1, byte2;

	int sx = scroll_x;// +(ppuctrl & 1 ? 256 : 0);
	int xMin = (sx / 8) + left8;
	int xMax = (sx + 256) / 8;


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

		if (ppuctrl == 0x89)
		{
			int yu = 0;
		}

		if (ppuctrl & 1)
			addr ^= 0x400;

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

				if (xp < 0 || xp >= 256 || yp < 0 || yp >= 240)
					continue;

				int bit0 = byte1 & 1 ? 1 : 0;
				int bit1 = byte2 & 1 ? 1 : 0;

				byte1 >>= 1;
				byte2 >>= 1;

				int colorindex = bit2 * 4 + (bit0 | bit1 * 2);

				gfxdata[yp * 256 + xp] = palettes[vram[0x3f00 + colorindex]];
				//ppu_draw_frame();
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
	int left8 = ppuctrl & 0x04 ? 1 : 0;

	u16 byte1, byte2;
	s16 y, tileid, att, x, i;

	int sprcount = 0;
	for (int j = 64; j > 0; j--)
	{
		i = j % 64;

		if (ppuoamdma == 0)
		{
			y = oammem[i * 4 + 0] + 1 & 0xff;
			tileid = oammem[i * 4 + 1];
			att = oammem[i * 4 + 2];
			x = oammem[i * 4 + 3] & 0xff + left8;
		}
		else
		{
			y = ram[oamaddr | i * 4 + 0] + 1;
			tileid = ram[oamaddr | i * 4 + 1];
			att = ram[oamaddr | i * 4 + 2];
			x = ram[oamaddr | i * 4 + 3];
		}

		if (att & frontback)
			continue;

		if (y >= 0xef || x >= 0xf9)
			continue;

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

				int xp = x + col;
				int yp = y + row;
				if (xp < 0 || xp >= 256 || yp < 0 || yp >= 240)
					continue;

				if (palindex != 0)
				{
					gfxdata[256 * (y + row) + x + col] = palettes[vram[paladdr + colorindex]];
				}
			}
		}

		//if (!(ppustatus & 0x40) && i == 0)
		//	ppu_set_sprite0();
	}
}

u8 get_background_pixel()
{
	int patternaddr = ppuctrl & 0x10 ? 0x0000 : 0x1000;

	return 0;
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