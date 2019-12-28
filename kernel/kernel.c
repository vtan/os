#include "kernel.h"

#include "Keyboard.h"
#include "Pic.h"
#include "Terminal.h"
#include "VgaText.h"
#include "multiboot2.h"
#include "string.h"

extern void* _kernel_start;
extern void* _kernel_end;

static void logMemory();
static void logRamdisk(uintptr_t ramdiskStart, uintptr_t ramdiskEnd);
static void findRamdisk(void* multibootInfo, uintptr_t* ramdiskStart, uintptr_t* ramdiskEnd);

void kernel_main(void* multibootInfo)
{
  VgaText_init();
  Terminal_init();
  Pic_init();
  Keyboard_init();

  logMemory();

  uintptr_t ramdiskStart, ramdiskEnd;
  findRamdisk(multibootInfo, &ramdiskStart, &ramdiskEnd);
  logRamdisk(ramdiskStart, ramdiskEnd);

  char* elfHeader = (char*) ramdiskStart;
  elfHeader[4] = '\n';
  elfHeader[5] = 0;
  Terminal_print(elfHeader);

  while(1) {
    __asm__("hlt");
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
