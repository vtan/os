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


static Keyboard* globalKeyboardDriver;

static VgaText globalVgaText;
static Terminal globalTerminal(globalVgaText);

extern "C"
void kernel_main(void* multibootInfo)
{
  // TODO: assumes this is after the ramdisk ends
  PageAllocator pageAllocator((void*) KERNEL_MEMORY_OFFSET + MBYTES(1) + KBYTES(512));
  PageDirectoryManager pageDirectoryManager(pageAllocator);
  ProcessLoader processLoader(pageAllocator, pageDirectoryManager);
  Pic_init();
  Keyboard keyboardDriver(globalTerminal);
  globalKeyboardDriver = &keyboardDriver;

  logMemory();

  uintptr_t ramdiskStart, ramdiskEnd;
  findRamdisk(multibootInfo, &ramdiskStart, &ramdiskEnd);
  logRamdisk(ramdiskStart, ramdiskEnd);

  struct Process process = { 0 };
  processLoader.load((void*) ramdiskStart, &process);
  kprintf("Process entry point:          0x%x\n", process.entryPoint);
  kprintf("Process user stack pointer:   0x%x\n", process.userStackPointer);
  kprintf("Process kernel stack pointer: 0x%x\n", process.kernelStackPointer);
  uint64_t result = Process_run(&process);
  KERNEL_PAGE_DIRECTORY.use();
  kprintf("Process result: %d\n", result);

  while(1) {
    __asm__("hlt");
  }
}

extern "C"
void kernel_exception(struct Kernel_InterruptStack* stack) {
  globalTerminal.setColor(VgaText::Color::LIGHT_RED, VgaText::Color::BLACK);
  kprintf("Exception, halting\n");
  globalTerminal.setColor(VgaText::Color::LIGHT_GREY, VgaText::Color::BLACK);

  kprintf("Interrupt number: 0x%x\n", stack->interruptNumber);
  kprintf("Error code: 0x%x\n", stack->errorCode);
}

extern "C"
void kernel_irq(struct Kernel_InterruptStack* stack) {
  // kprintf("Entering interrupt handler, stack: 0x%x, return rsp: 0x%x\n", stack, stack->rsp);
  if (stack->interruptNumber == 0x21) {
    globalKeyboardDriver->handleIrq();
  }
}

void kprintf(const char* format, ...) {
  static char str[1024]; // Let's hope this is enough.

  va_list args;
  va_start(args, format);
  String_vprintf(str, format, args);
  va_end(args);

  globalTerminal.print(str);
}

static void logMemory() {
  const uint64_t kernelStart = (uintptr_t) &_kernel_start;
  const uint64_t kernelEnd = (uintptr_t) &_kernel_end;

  globalTerminal.setColor(VgaText::Color::LIGHT_BROWN, VgaText::Color::BLACK);
  kprintf("Kernel start: 0x%x\n", kernelStart);
  kprintf("Kernel end:   0x%x\n", kernelEnd);
  kprintf("Kernel size:  %d kB\n", (kernelEnd - kernelStart) / 1024);
  globalTerminal.setColor(VgaText::Color::LIGHT_GREY, VgaText::Color::BLACK);
}

static void logRamdisk(uintptr_t ramdiskStart, uintptr_t ramdiskEnd) {
  globalTerminal.setColor(VgaText::Color::LIGHT_BROWN, VgaText::Color::BLACK);
  kprintf("Ramdisk start: 0x%x\n", ramdiskStart);
  kprintf("Ramdisk end:   0x%x\n", ramdiskEnd);
  kprintf("Ramdisk size:  %d B\n", ramdiskEnd - ramdiskStart);
  globalTerminal.setColor(VgaText::Color::LIGHT_GREY, VgaText::Color::BLACK);
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
