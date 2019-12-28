#include "kernel.h"

#include "Keyboard.h"
#include "VgaText.h"

void Interrupt_genericExceptionHandler(struct Kernel_InterruptStack* stack) {
  Terminal_setColor(VgaText_LIGHT_RED, VgaText_BLACK);
  kprintf("Exception, halting\n");
  Terminal_setColor(VgaText_LIGHT_GREY, VgaText_BLACK);

  kprintf("Interrupt number: 0x%x\n", stack->interruptNumber);
  kprintf("Error code: 0x%x\n", stack->errorCode);
}

void Interrupt_genericIrqHandler(struct Kernel_InterruptStack* stack) {
  if (stack->interruptNumber == 0x21) {
    Keyboard_handler();
  }
}
