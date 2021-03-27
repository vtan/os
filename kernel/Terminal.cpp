#include "Terminal.hpp"
#include "VgaText.hpp"

Terminal::Terminal(VgaText& vt) : vgaText(vt) {
  this->offset = 0;
  this->attributes = VgaText::colorFrom(VgaText::Color::LIGHT_GREY, VgaText::Color::BLACK);
}

void Terminal::clear() {
  this->attributes = VgaText::colorFrom(VgaText::Color::LIGHT_GREY, VgaText::Color::BLACK);

  for (size_t i = 0; i < VgaText::WIDTH * VgaText::HEIGHT; ++i) {
    this->vgaText.put(i, this->attributes, ' ');
  }
  this->vgaText.moveCursor(0);
}

void Terminal::print(const char* str) {
  if (*str == 0) {
    return;
  }

  while (*str != 0) {
    if (*str == '\n') {
      const size_t x = this->offset % VgaText::WIDTH;
      this->offset += VgaText::WIDTH - x;
    } else {
      this->vgaText.put(this->offset, this->attributes, *str);
      ++this->offset;
    }

    if (this->offset >= VgaText::SCREEN_SIZE) {
      this->vgaText.scrollUp();
      this->offset -= VgaText::WIDTH;
    }

    ++str;
  }
  this->vgaText.moveCursor(this->offset);
}

void Terminal::setColor(VgaText::Color fg, VgaText::Color bg) {
  this->attributes = VgaText::colorFrom(fg, bg);
}
