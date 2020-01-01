#pragma once

#include "VgaText.h"

void Terminal_init();
void Terminal_print(const char* str);
void Terminal_setColor(enum VgaText_Color fg, enum VgaText_Color bg);
