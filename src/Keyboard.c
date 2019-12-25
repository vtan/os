#include "Keyboard.h"
#include "Pic.h"
#include "Port.h"
#include "Terminal.h"

#define Keyboard_PORT 0x60

void Keyboard_init() {
  uint8_t picInterruptMask = Port_in(Pic_MASTER_DATA);
  Port_out(Pic_MASTER_DATA, picInterruptMask & 0xFD);
}

void Keyboard_handler() {
  uint8_t scanCode = Port_in(Keyboard_PORT);

  char str[4];
  str[0] = "0123456789ABCDEF"[(scanCode >> 4) & 0xF];
  str[1] = "0123456789ABCDEF"[scanCode & 0xF];
  str[2] = '\n';
  str[3] = 0;
  Terminal_print(str);

  Port_out(Pic_MASTER_COMMAND, Pic_END_OF_INTERRUPT);
}
