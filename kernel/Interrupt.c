#include "Keyboard.h"
#include "Terminal.h"
#include "VgaText.h"
#include "kernel.h"
#include "string.h"

void Interrupt_genericExceptionHandler(struct Kernel_InterruptStack* stack) {
  Terminal_setColor(VgaText_LIGHT_RED, VgaText_BLACK);
  Terminal_print("Exception, halting\n");
  Terminal_setColor(VgaText_LIGHT_GREY, VgaText_BLACK);

  char str[19] = "0x";
  String_uformat(stack->interruptNumber, str + 2, 16);
  Terminal_print("Interrupt number: ");
  Terminal_print(str);
  String_uformat(stack->errorCode, str + 2, 16);
  Terminal_print("\nError code: ");
  Terminal_print(str);
}

void Interrupt_genericIrqHandler(struct Kernel_InterruptStack* stack) {
  if (stack->interruptNumber == 0x21) {
    Keyboard_handler();
  }
}
