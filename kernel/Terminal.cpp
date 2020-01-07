#include "Terminal.hpp"
#include "VgaText.hpp"

struct Terminal_State {
  size_t offset;
  uint8_t attributes;
};

static struct Terminal_State state;

void Terminal_init() {
  state.offset = 0;
  state.attributes = VgaText_color(VgaText_LIGHT_GREY, VgaText_BLACK);

  for (size_t i = 0; i < VgaText_WIDTH * VgaText_HEIGHT; ++i) {
    VgaText_put(i, state.attributes, ' ');
  }
  VgaText_moveCursor(0);
}

void Terminal_print(const char* str) {
  if (*str == 0) {
    return;
  }

  while (*str != 0) {
    if (*str == '\n') {
      const size_t x = state.offset % VgaText_WIDTH;
      state.offset += VgaText_WIDTH - x;
    } else {
      VgaText_put(state.offset, state.attributes, *str);
      ++state.offset;
    }

    if (state.offset >= VgaText_PAGE_SIZE) {
      VgaText_scrollUp();
      state.offset -= VgaText_WIDTH;
    }

    ++str;
  }
  VgaText_moveCursor(state.offset);
}

void Terminal_setColor(enum VgaText_Color fg, enum VgaText_Color bg) {
  state.attributes = VgaText_color(fg, bg);
}
