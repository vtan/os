#pragma once

#include "VgaText.hpp"

class Terminal {
  VgaText& vgaText;
  size_t offset;
  uint8_t attributes;

public:
  Terminal(VgaText&);

  void print(const char*);
  void setColor(VgaText::Color fg, VgaText::Color bg);
};
