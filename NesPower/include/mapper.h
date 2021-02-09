#pragma once

#include "types.h"

u8 ram[0x10000];
u8 vram[0x4000];
u8 oam[0x100];
u8* rom;

bool load_rom(char* filename);

void set_mapper();

u16 cpu_read_word(u16 addr);

u8 cpu_read_debug(u16 addr);

u8 cpu_read(u16 addr);

void cpu_write(u16 addr, u8 v);

u8 ppu_read(u16 addr);

void ppu_read_8_bytes(u16 addr, u8* bytearr);

void ppu_write(u16 addr, u8 v);

void mapper_clean();