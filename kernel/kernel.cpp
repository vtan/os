#include "kernel.hpp"

#include "Keyboard.hpp"
#include "PageAlloc.hpp"
#include "PageDirectory.hpp"
#include "Pic.hpp"
#include "Process.hpp"
#include "ProcessExecutor.hpp"
#include "SerialDevice.hpp"
#include "String.hpp"
#include "TarFilesystem.hpp"
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

static Keyboard* globalKeyboardDriver = nullptr;
static SerialDevice* globalSerialDevice = nullptr;

static VgaText globalVgaText;
static Terminal globalTerminal(globalVgaText);

extern "C"
void kernel_main(void* multibootInfo)
{
  mapPhysicalMemoryToKernel();
  globalTerminal.clear();

  // TODO: this assumes the ramdisk ends before 2MB
  PageAllocator pageAllocator(PhysicalAddress { MBYTES(2) });
  PageDirectoryManager pageDirectoryManager(pageAllocator);
  ProcessLoader processLoader(pageAllocator, pageDirectoryManager);

  Pic::initialize();
  VgaText::initializeCursor();
  enableInterrupts();
  SerialDevice serialDevice;
  globalSerialDevice = &serialDevice;
  Keyboard keyboardDriver(globalTerminal);
  globalKeyboardDriver = &keyboardDriver;

  logMemory();

  uintptr_t ramdiskStart, ramdiskEnd;
  findRamdisk(multibootInfo, &ramdiskStart, &ramdiskEnd);
  logRamdisk(ramdiskStart, ramdiskEnd);

  TarFilesystem tarFilesystem((void*) ramdiskStart);
  tarFilesystem.listFiles();

  {
    int i = 0;
    auto file = tarFilesystem.start;
    while (file != nullptr) {
      Process* process = ProcessExecutor::allocate();
      kprintf("Loading process %d\n", i);
      processLoader.load(file->fileContent(), process);
      kprintf("Process entry point:          %p\n", process->entryPoint);
      kprintf("Process user stack pointer:   %p\n", process->userStackPointer);
      kprintf("Process kernel stack pointer: %p\n", process->kernelStackPointer);

      ++i;
      file = file->next();
    }
  }

  ProcessExecutor::switchToNext();

  while(1) {
    __asm__("hlt");
  }
}

extern "C"
void kernel_exception(InterruptFrame* frame) {
  globalTerminal.setColor(VgaText::Color::LIGHT_RED, VgaText::Color::BLACK);
  kprintf("Exception, halting\n");
  globalTerminal.setColor(VgaText::Color::LIGHT_GREY, VgaText::Color::BLACK);

  kprintf("Interrupt number: 0x%x\n", frame->interruptNumber);
  kprintf("Error code: 0x%x\n", frame->errorCode);
}

extern "C"
void kernel_irq(InterruptFrame* frame) {
  // kprintf("Entering interrupt handler, stack: %p, return rsp: %p\n", frame, frame->rsp);
  if (frame->interruptNumber == 0x21) {
    globalKeyboardDriver->handleIrq();
  }
}

extern "C"
uint64_t kernel_syscall(SyscallFrame* frame) {
  // kprintf("Entering syscall handler, stack: %p, return rsp: %p, return rip: %p\n", frame, frame->rsp, frame->rip);
  switch (frame->syscallNumber) {
    case 0:
      kprintf("Process exited, return value: 0x%x\n", frame->syscallArg);
      ProcessExecutor::switchToNext();
      while(1) { __asm__("hlt"); }
    case 0x100:
      kprintf("Process output: 0x%x\n", frame->syscallArg);
      return 0;
    default:
      kprintf("panic: Unknown syscall number %d\n", frame->syscallNumber);
      kernel_halt();
      return 0;
  }
}

void kprintf(const char* format, ...) {
  char str[64];
  const char* const end = str + 64;
  char* p = str;

  auto flush = [&]() {
    *p = 0;
    if (globalSerialDevice) {
      globalSerialDevice->writeString(str);
    }
    globalTerminal.print(str);
  };
  auto putChar = [&](char c) {
    *p++ = c;
    if (p == end - 1) {
      flush();
      p = str;
    }
  };

  va_list args;
  va_start(args, format);
  String::vformat(putChar, format, args);
  va_end(args);

  if (p > str) {
    flush();
  }
}

static void logMemory() {
  const uint64_t kernelStart = (uintptr_t) &_kernel_start;
  const uint64_t kernelEnd = (uintptr_t) &_kernel_end;

  globalTerminal.setColor(VgaText::Color::LIGHT_BROWN, VgaText::Color::BLACK);
  kprintf("Kernel start: %p\n", kernelStart);
  kprintf("Kernel end:   %p\n", kernelEnd);
  kprintf("Kernel size:  %d kB\n", (kernelEnd - kernelStart) / 1024);
  globalTerminal.setColor(VgaText::Color::LIGHT_GREY, VgaText::Color::BLACK);
}

static void logRamdisk(uintptr_t ramdiskStart, uintptr_t ramdiskEnd) {
  globalTerminal.setColor(VgaText::Color::LIGHT_BROWN, VgaText::Color::BLACK);
  kprintf("Ramdisk start: %p\n", ramdiskStart);
  kprintf("Ramdisk end:   %p\n", ramdiskEnd);
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
    start += KERNEL_STATIC_MEMORY_OFFSET;
    end += KERNEL_STATIC_MEMORY_OFFSET;
  }

  *ramdiskStart = start;
  *ramdiskEnd = end;
}
