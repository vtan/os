#pragma once

#include "kernel.hpp"

class VgaText {
  uint16_t* buffer = (uint16_t*) PhysicalAddress { 0xB'8000 }.toVirtual();

public:
  enum class Color : uint8_t {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    BROWN = 6,
    LIGHT_GREY = 7,
    DARK_GREY = 8,
    LIGHT_BLUE = 9,
    LIGHT_GREEN = 10,
    LIGHT_CYAN = 11,
    LIGHT_RED = 12,
    LIGHT_MAGENTA = 13,
    LIGHT_BROWN = 14,
    WHITE = 15,
  };

  static constexpr int WIDTH = 80;
  static constexpr int HEIGHT = 25;
  static constexpr int SCREEN_SIZE = WIDTH * HEIGHT;

  VgaText();

  void put(size_t offset, uint8_t attributes, uint8_t character);
  void scrollUp();
  void moveCursor(size_t offset);

  static inline uint8_t colorFrom(Color foreground, Color background) {
    return (uint8_t) foreground | (uint8_t) background << 4;
  }
};
