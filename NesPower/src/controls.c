#include "../include/controls.h"

void controls_init()
{
	buttonid = 1;
}

u8 get_keys(u8* keys, u8 id)
{
	memset(&key, 0, 8);

	switch (id)
	{
	case 0:
		return keys[SDL_SCANCODE_Z];
	case 1:
		return keys[SDL_SCANCODE_X];
	case 2:
		return keys[SDL_SCANCODE_SPACE];
	case 3:
		return keys[SDL_SCANCODE_RETURN];
	case 4:
		return keys[SDL_SCANCODE_UP];
	case 5:
		return keys[SDL_SCANCODE_DOWN];
	case 6:
		return keys[SDL_SCANCODE_LEFT];
	case 7:
		return keys[SDL_SCANCODE_RIGHT];
	}
	return 0;
}

void controls_write(u8 v)
{
	strobe = v & 1;

	if (strobe)
	{
		buttonid = 0;
	}
}

u8 controls_read()
{
	u8* keys = (u8*)SDL_GetKeyboardState(NULL);
	u8 val = 0x40;

	if (!strobe && buttonid >= 0)
	{
		if (buttonid >= 8)
		{
			buttonid = 0;
		}

		if (get_keys(keys, buttonid))
		{
			val = 0x41;
		}
		buttonid++;
	}
	return val;
}