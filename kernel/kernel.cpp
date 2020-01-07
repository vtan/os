#include "kernel.hpp"

#include "Keyboard.hpp"
#include "PageAlloc.hpp"
#include "PageDirectory.hpp"
#include "Pic.hpp"
#include "Process.hpp"
#include "String.hpp"
#include "Terminal.hpp"
#include "VgaText.hpp"

extern "C" {
#include "multiboot2.h"
}

extern void* _kernel_start;
extern void* _kernel_end;

static void logMemory();
static void logRamdisk(uintptr_t ramdiskStart, uintptr_t ramdiskEnd);
static void findRamdisk(void* multibootInfo, uintptr_t* ramdiskStart, uintptr_t* ramdiskEnd);

// TODO temporary
extern uint64_t* kernel_page_table_l4;

extern "C"
void kernel_main(void* multibootInfo)
{
  VgaText_init();
  Terminal_init();
  PageAlloc_init((void*) KERNEL_MEMORY_OFFSET + MBYTES(1) + KBYTES(512));
  Pic_init();
  Keyboard_init();

  logMemory();

  uintptr_t ramdiskStart, ramdiskEnd;
  findRamdisk(multibootInfo, &ramdiskStart, &ramdiskEnd);
  logRamdisk(ramdiskStart, ramdiskEnd);

  struct Process process = { 0 };
  Process_load((void*) ramdiskStart, &process);
  kprintf("Process entry point: 0x%x\n", process.entryPoint);
  kprintf("Process stack top: 0x%x\n", process.stackTop);
  uint64_t result = Process_run(&process);
  PageDirectory_use((uint64_t*) (((uintptr_t) &kernel_page_table_l4) - KERNEL_MEMORY_OFFSET));
  kprintf("Process result: %d\n", result);

  while(1) {
    __asm__("hlt");
  }
}

extern "C"
void kernel_exception(struct Kernel_InterruptStack* stack) {
  Terminal_setColor(VgaText_LIGHT_RED, VgaText_BLACK);
  kprintf("Exception, halting\n");
  Terminal_setColor(VgaText_LIGHT_GREY, VgaText_BLACK);

  kprintf("Interrupt number: 0x%x\n", stack->interruptNumber);
  kprintf("Error code: 0x%x\n", stack->errorCode);
}

extern "C"
void kernel_irq(struct Kernel_InterruptStack* stack) {
  if (stack->interruptNumber == 0x21) {
    Keyboard_handler();
  }
}

void kprintf(const char* format, ...) {
  static char str[1024]; // Let's hope this is enough.

  va_list args;
  va_start(args, format);
  String_vprintf(str, format, args);
  va_end(args);

  Terminal_print(str);
}

static void logMemory() {
  const uint64_t kernelStart = (uintptr_t) &_kernel_start;
  const uint64_t kernelEnd = (uintptr_t) &_kernel_end;

  Terminal_setColor(VgaText_LIGHT_BROWN, VgaText_BLACK);
  kprintf("Kernel start: 0x%x\n", kernelStart);
  kprintf("Kernel end:   0x%x\n", kernelEnd);
  kprintf("Kernel size:  %d kB\n", (kernelEnd - kernelStart) / 1024);
  Terminal_setColor(VgaText_LIGHT_GREY, VgaText_BLACK);
}

static void logRamdisk(uintptr_t ramdiskStart, uintptr_t ramdiskEnd) {
  Terminal_setColor(VgaText_LIGHT_BROWN, VgaText_BLACK);
  kprintf("Ramdisk start: 0x%x\n", ramdiskStart);
  kprintf("Ramdisk end:   0x%x\n", ramdiskEnd);
  kprintf("Ramdisk size:  %d B\n", ramdiskEnd - ramdiskStart);
  Terminal_setColor(VgaText_LIGHT_GREY, VgaText_BLACK);
}

static void findRamdisk(void* multibootInfo, uintptr_t* ramdiskStart, uintptr_t* ramdiskEnd) {
  uintptr_t start = 0;
  uintptr_t end = 0;
  for (
    struct multiboot_tag* tag = (struct multiboot_tag*) (multibootInfo + 8);
    tag->type != MULTIBOOT_TAG_TYPE_END;
    tag = (struct multiboot_tag*) ((uint8_t*) tag + ((tag->size + 7) & ~7))
  ) {
    if (tag-> type == MULTIBOOT_TAG_TYPE_MODULE) {
      struct multiboot_tag_module* module = (struct multiboot_tag_module*) tag;
      start = module->mod_start;
      end = module->mod_end;
    }
  }

  if (start != 0) {
    start += KERNEL_MEMORY_OFFSET;
    end += KERNEL_MEMORY_OFFSET;
  }

  *ramdiskStart = start;
  *ramdiskEnd = end;
}
