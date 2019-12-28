#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "Keyboard.h"
#include "Pic.h"
#include "Terminal.h"
#include "VgaText.h"
#include "kernel.h"
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

static void logMemory() {
	const uint64_t kernelStart = (uintptr_t) &_kernel_start;
	const uint64_t kernelEnd = (uintptr_t) &_kernel_end;

	Terminal_setColor(VgaText_LIGHT_BROWN, VgaText_BLACK);
	char str[19] = "0x";

	String_uformat(kernelStart, str + 2, 16);
	Terminal_print("Kernel start: ");
	Terminal_print(str);
	Terminal_print("\n");

	String_uformat(kernelEnd, str + 2, 16);
	Terminal_print("Kernel end:   ");
	Terminal_print(str);
	Terminal_print("\n");

	String_uformat((kernelEnd - kernelStart) / 1024, str, 10);
	Terminal_print("Kernel size:  ");
	Terminal_print(str);
	Terminal_print(" kB\n\n");

	Terminal_setColor(VgaText_LIGHT_GREY, VgaText_BLACK);
}

static void logRamdisk(uintptr_t ramdiskStart, uintptr_t ramdiskEnd) {
	Terminal_setColor(VgaText_LIGHT_BROWN, VgaText_BLACK);
	char str[19] = "0x";

	String_uformat(ramdiskStart, str + 2, 16);
	Terminal_print("Ramdisk start: ");
	Terminal_print(str);
	Terminal_print("\n");

	String_uformat(ramdiskEnd, str + 2, 16);
	Terminal_print("Ramdisk end:   ");
	Terminal_print(str);
	Terminal_print("\n");

	String_uformat(ramdiskEnd - ramdiskStart, str, 10);
	Terminal_print("Ramdisk size:  ");
	Terminal_print(str);
	Terminal_print(" B\n\n");

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
