#pragma once

#include "types.h"

u32 scanline;

u8 ppuctrl;
u8 ppumask;
u8 ppustatus;
u8 ppuoamaddr;
u8 ppuoamdata;
u8 ppuscroll;
u8 ppuaddr;
u8 ppudata;

u16 v;
bool latchtoggle;
bool scrolltoggle;

u8 ppu_dummy2007;

u16 scroll_x;
u8 scroll_y;

u16 v;
u16 t;
u8 x;
u8 w;

int nametableaddr;
int patternaddr;

u8 framefinished;

u32 cycle;

u8 mirrornametable;

u8 ppuoamdma;

u8 oammem[256];

u8 ppu_nmi;

u32 gfxdata[(512 * 240)];

//u8 gfxdata[256 * 240];

u32 ppugfxdata[2][128 * 128];

u32 palettes[64];

void ppu_init();

void ppu_draw_frame();

void ppu_set_nmi();

void ppu_set_vblank();

void ppu_clear_vblank();

void ppu_set_sprite0();

void ppu_clear_sprite0();

void ppu_render();

void ppu_render_background(u16 nameaddr);

void ppu_render_background_bak();

void ppu_render_sprites(u8 frontback);

int get_attribute_index(int x, int y, int attrib);

void write_ppu_ctrl(u8 v);

void increase_x();

void ppu_clean();
