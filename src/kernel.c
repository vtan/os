#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "Keyboard.h"
#include "Pic.h"
#include "Terminal.h"
#include "VgaText.h"

void kernel_main(void)
{
	VgaText_init();
	Terminal_init();
	Pic_init();
	Keyboard_init();

	while(1) {
		__asm__("hlt");
	}
}
