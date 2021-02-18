#pragma once

#include "stdio.h"
#include "SDL2/SDL.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef char s8;
typedef short s16;
typedef int s32;

typedef int bool;
#define true 1
#define false 0

#define APP_WIDTH   256
#define APP_HEIGHT  240

#define APP_SCALE 3

SDL_Renderer* renderer;
SDL_Window* window;
SDL_Texture* screen;