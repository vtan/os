#include <stdint.h>

#include "Idt.h"

struct Idt_Record {
   uint16_t offsetLow;
   uint16_t selector;
   uint8_t istOffset;
   uint8_t type;
   uint16_t offsetMid;
   uint32_t offsetHigh;
   uint32_t zero;
} __attribute__((__packed__));

struct Idt_Record idt[256];

void interrupt_handler(void);
void Keyboard_interrupt(void);

void Idt_init() {
  for (int i = 0; i < 256; ++i) {
    uintptr_t handler = (uintptr_t) (i == 0x21 ? Keyboard_interrupt : interrupt_handler);
    idt[i].offsetLow = handler & 0xFFFF;
    idt[i].offsetMid = (handler >> 16) & 0xFFFF;
    idt[i].offsetHigh = handler >> 32;
    idt[i].selector = 0x08;
    idt[i].type = 0x8E;
    idt[i].istOffset = 0;
    idt[i].zero = 0;
  }
}
