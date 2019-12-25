#include <stdint.h>

#include "Idt.h"

struct Idt_Record {
   uint16_t offsetLow;
   uint16_t selector;
   uint8_t unused;
   uint8_t type;
   uint16_t offsetHigh;
} __attribute__((__packed__));

struct Idt_Pointer {
  uint16_t limit;
  uint32_t base;
} __attribute__((__packed__));

struct Idt_Record Idt_table[256];
struct Idt_Pointer Idt_pointer;

void interrupt_handler(void);
void Keyboard_interrupt(void);

void Idt_init() {
  for (int i = 0; i < 256; ++i) {
    uintptr_t handler = (uintptr_t) (i == 0x21 ? Keyboard_interrupt : interrupt_handler);
    Idt_table[i].offsetLow = handler & 0xFFFF;
    Idt_table[i].offsetHigh = (handler >> 16) & 0xFFFF;
    Idt_table[i].selector = 0x08;
    Idt_table[i].type = 0x8E;
    Idt_table[i].unused = 0;
  }
  Idt_pointer.base = (uintptr_t) Idt_table;
  Idt_pointer.limit = 256 * sizeof(struct Idt_Record) - 1;
}
