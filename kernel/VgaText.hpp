#pragma once

#include <stddef.h>
#include <stdint.h>

#define VgaText_WIDTH 80
#define VgaText_HEIGHT 25
#define VgaText_PAGE_SIZE (VgaText_WIDTH * VgaText_HEIGHT)

enum VgaText_Color {
  VgaText_BLACK = 0,
  VgaText_BLUE = 1,
  VgaText_GREEN = 2,
  VgaText_CYAN = 3,
  VgaText_RED = 4,
  VgaText_MAGENTA = 5,
  VgaText_BROWN = 6,
  VgaText_LIGHT_GREY = 7,
  VgaText_DARK_GREY = 8,
  VgaText_LIGHT_BLUE = 9,
  VgaText_LIGHT_GREEN = 10,
  VgaText_LIGHT_CYAN = 11,
  VgaText_LIGHT_RED = 12,
  VgaText_LIGHT_MAGENTA = 13,
  VgaText_LIGHT_BROWN = 14,
  VgaText_WHITE = 15,
};

void VgaText_init();
void VgaText_put(size_t offset, uint8_t attributes, uint8_t character);
void VgaText_scrollUp();
void VgaText_moveCursor(size_t offset);

uint8_t VgaText_color(enum VgaText_Color foreground, enum VgaText_Color background);
