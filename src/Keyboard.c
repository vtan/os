#include "Keyboard.h"
#include "Pic.h"
#include "Port.h"

#define Keyboard_PORT 0x60

void Keyboard_init() {
  uint8_t picInterruptMask = Port_in(Pic_MASTER_DATA);
  Port_out(Pic_MASTER_DATA, picInterruptMask & 0xFD);
}

void Keyboard_handler() {
  uint8_t keycode = Port_in(Keyboard_PORT);
  uint8_t* vga = (uint8_t*) 0xB8000;
  vga[0] = "0123456789ABCDEF"[(keycode >> 4) & 0xF];
  vga[2] = "0123456789ABCDEF"[keycode & 0xF];

  Port_out(Pic_MASTER_COMMAND, Pic_END_OF_INTERRUPT);
}
