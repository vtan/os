#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "Keyboard.h"
#include "Pic.h"
#include "Terminal.h"
#include "VgaText.h"
#include "string.h"

extern void* _kernel_start;
extern void* _kernel_end;

void kernel_main(void)
{
	VgaText_init();
	Terminal_init();
	Pic_init();
	Keyboard_init();

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

	while(1) {
		__asm__("hlt");
	}
}
