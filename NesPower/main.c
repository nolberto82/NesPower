#include "include/cpu.h"
#include "include/ppu.h"
#include "include/mapper.h"
#include "include/controls.h"

void sdl_init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	window = SDL_CreateWindow("Nes Power", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, APP_WIDTH * APP_SCALE, APP_HEIGHT * APP_SCALE, 0);

	if (!window)
	{
		printf("Failed to open %d x %d window: %s\n", APP_WIDTH, APP_HEIGHT, SDL_GetError());
		exit(1);
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (!renderer)
	{
		printf("Failed to create renderer: %s\n", SDL_GetError());
		exit(1);
	}

	//SDL_RenderSetScale(renderer, APP_SCALE, APP_SCALE * 0.03);
}

void sdl_input(void)
{
	SDL_Event event;
	//u8* keys = (u8*)SDL_GetKeyboardState(NULL);

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			app_running = false;
			break;
		}
	}
}

int main()
{
	atexit(mapper_clean);
	atexit(cpu_clean);

	sdl_init();

	char* romname = { "../../../../tests/smb.nes" };

	if (!load_rom(romname))
	{
		exit(1);
	}

	cpu_init(romname);
	ppu_init();
	controls_init();

	app_running = true;

	while (app_running)
	{
		cpu_step();

		//for (int i = 0; i < 3; i++)
		ppu_step();

		sdl_input();
	}

	SDL_DestroyTexture(screen);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
