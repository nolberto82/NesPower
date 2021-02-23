#pragma once

#include "types.h"

u32 ppu_scanline;
u32 ppu_dots;
u8 ppu_cyc;

u8 ppuctrl;
u8 ppumask;
u8 ppustatus;
u8 ppuoamaddr;
u8 ppuoamdata;
u8 ppuscroll;
u8 ppuaddr;
u8 ppudata;

bool background8px;
bool sprites8px;
bool backgroundrender;
bool spritesrender;

bool latchtoggle;
bool scrolltoggle;

u8 ppu_dummy2007;

u8 scroll_x;
u8 scroll_y;

u16 ppu_v;
u16 ppu_t;
u16 ppu_x;
u8 ppu_w;

int nametableaddr;
int patternaddr;

u8 framefinished;

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

void ppu_step();

void ppu_render_background();

void ppu_render_nametables();

void ppu_render_sprites(u8 frontback);

u8 get_background_pixel();

int get_attribute_index(int x, int y, int attrib);

void ppu_ctrl_write(u8 val);

void ppu_mask_write(u8 val);

void ppu_scroll_write(u8 val);

void ppu_addr_write(u8 val);

void ppu_data_write(u8 val);

void ppu_increase_v();

void ppu_render_background_new();

void ppu_clean();
