#include "Terminal.h"
#include "VgaText.h"

struct Terminal_State {
  size_t offset;
  uint8_t attributes;
};

struct Terminal_State Terminal_state;

void Terminal_init() {
  Terminal_state.offset = 0;
  Terminal_state.attributes = VgaText_color(VgaText_LIGHT_GREY, VgaText_BLACK);

  for (size_t i = 0; i < VgaText_WIDTH * VgaText_HEIGHT; ++i) {
    VgaText_put(i, Terminal_state.attributes, ' ');
  }
  VgaText_moveCursor(0);
}

void Terminal_print(const char* str) {
  if (*str == 0) {
    return;
  }

  while (*str != 0) {
    if (*str == '\n') {
      const size_t x = Terminal_state.offset % VgaText_WIDTH;
      Terminal_state.offset += VgaText_WIDTH - x;
    } else {
      VgaText_put(Terminal_state.offset, Terminal_state.attributes, *str);
      ++Terminal_state.offset;
    }
    ++str;
  }
  VgaText_moveCursor(Terminal_state.offset);
}
