#pragma once

#include "types.h"

u8 buttonid;

bool strobe;

typedef struct
{
	u8 a;
	u8 b;
	u8 select;
	u8 start;
	u8 up;
	u8 down;
	u8 left;
	u8 right;
}Keys;

Keys key;

void controls_init();

u8 get_keys(u8* keys, u8 id);

void controls_write(u8 v);

u8 controls_read();
