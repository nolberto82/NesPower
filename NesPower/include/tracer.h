#pragma once

#include "types.h"

void trace_disasm(u16 pc, u8 op, u8 x, u8 y, u8 sp, int cycles, char* strtrace);
