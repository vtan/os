#include "Keyboard.h"
#include "kernel.h"

void Interrupt_genericExceptionHandler(struct Kernel_InterruptStack* stack) {
}

void Interrupt_genericIrqHandler(struct Kernel_InterruptStack* stack) {
  if (stack->interruptNumber == 0x21) {
    Keyboard_handler();
  }
}
