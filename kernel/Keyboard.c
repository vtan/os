#include "kernel.h"

#include "Keyboard.h"
#include "Pic.h"
#include "Port.h"
#include "Terminal.h"

#define Keyboard_PORT 0x60

const char keyChars[256] = {
  0, '\e', '1', '2', '3', '4', '5', '6',
  '7', '8', '9', '0', '-', '=', '\b', '\t',
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
  'o', 'p', '[', ']', '\n', 0, 'a', 's',
  'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
  '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
  'b', 'n', 'm', ',', '.', '/', 0, 0,
  0, ' ',

  [128] = 0, '\e', '!', '@', '#', '$', '%', '^',
  '&', '*', '(', ')', '_', '+', '\b', '\t',
  'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
  'O', 'P', '{', '}', '\n', 0, 'A', 'S',
  'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
  '"', '~', 0, '|', 'Z', 'X', 'C', 'V',
  'B', 'N', 'M', '<', '>', '?', 0, 0,
  0, ' '
};

enum Keyboard_Modifier {
  Keyboard_LEFT_SHIFT = 1,
  Keyboard_RIGHT_SHIFT = 1 << 1
};

struct Keyboard_State {
  uint8_t modifiers;
  int ignoreBytes;
};

static struct Keyboard_State state = {
  .modifiers = 0,
  .ignoreBytes = 0
};

void Keyboard_init() {
  uint8_t picInterruptMask = Port_in(Pic_MASTER_DATA);
  Port_out(Pic_MASTER_DATA, picInterruptMask & 0xFD);
}

void Keyboard_handler() {
  const uint8_t scanCode = Port_in(Keyboard_PORT);

  if (state.ignoreBytes > 0) {
    --state.ignoreBytes;
  } else if (scanCode == 0xE0) {
    state.ignoreBytes = 1;
  } else if (scanCode == 0xE1) {
    state.ignoreBytes = 2;
  } else if (scanCode == 0x2A) {
    state.modifiers |= Keyboard_LEFT_SHIFT;
  } else if (scanCode == 0x36) {
    state.modifiers |= Keyboard_RIGHT_SHIFT;
  } else if (scanCode == 0xAA) {
    state.modifiers &= ~Keyboard_LEFT_SHIFT;
  } else if (scanCode == 0xB6) {
    state.modifiers &= ~Keyboard_RIGHT_SHIFT;
  } else if (scanCode < 0x80) {
    const size_t charIndex =
      state.modifiers & (Keyboard_LEFT_SHIFT | Keyboard_RIGHT_SHIFT)
        ? scanCode + 128
        : scanCode;
    char str[2];
    str[0] = keyChars[charIndex];
    str[1] = 0;
    Terminal_print(str);
  }

  Port_out(Pic_MASTER_COMMAND, Pic_END_OF_INTERRUPT);
}
