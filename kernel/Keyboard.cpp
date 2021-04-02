#include "kernel.hpp"

#include "Keyboard.hpp"
#include "Pic.hpp"
#include "Port.hpp"
#include "Terminal.hpp"

constexpr uint8_t PORT = 0x60;

static const char keyChars[128] = {
  0, '\e', '1', '2', '3', '4', '5', '6',
  '7', '8', '9', '0', '-', '=', '\b', '\t',
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
  'o', 'p', '[', ']', '\n', 0, 'a', 's',
  'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
  '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
  'b', 'n', 'm', ',', '.', '/', 0, 0,
  0, ' '
};

static const char keyCharsShift[128] = {
  0, '\e', '!', '@', '#', '$', '%', '^',
  '&', '*', '(', ')', '_', '+', '\b', '\t',
  'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
  'O', 'P', '{', '}', '\n', 0, 'A', 'S',
  'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
  '"', '~', 0, '|', 'Z', 'X', 'C', 'V',
  'B', 'N', 'M', '<', '>', '?', 0, 0,
  0, ' '
};

enum Modifier {
  LEFT_SHIFT = 1,
  RIGHT_SHIFT = 1 << 1
};

Keyboard::Keyboard(Terminal& t) : terminal(t) {
  constexpr uint8_t irqLine = 1;
  Pic::enableInterrupt(irqLine);
}

void Keyboard::handleIrq() {
  const uint8_t scanCode = port_in8(PORT);

  if (this->ignoreBytes > 0) {
    --this->ignoreBytes;
  } else if (scanCode == 0xE0) {
    this->ignoreBytes = 1;
  } else if (scanCode == 0xE1) {
    this->ignoreBytes = 2;
  } else if (scanCode == 0x2A) {
    this->modifiers |= LEFT_SHIFT;
  } else if (scanCode == 0x36) {
    this->modifiers |= RIGHT_SHIFT;
  } else if (scanCode == 0xAA) {
    this->modifiers &= ~LEFT_SHIFT;
  } else if (scanCode == 0xB6) {
    this->modifiers &= ~RIGHT_SHIFT;
  } else if (scanCode < 0x80) {
    const char ch =
      this->modifiers & (LEFT_SHIFT | RIGHT_SHIFT)
        ? keyCharsShift[scanCode]
        : keyChars[scanCode];
    char str[2];
    str[0] = ch;
    str[1] = 0;
    this->terminal.print(str);
  }
}
