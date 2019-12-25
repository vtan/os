#include "Port.h"
#include "VgaText.h"

uint16_t* VgaText_buffer = (uint16_t*) 0xB8000;

void VgaText_put(size_t offset, uint8_t attributes, uint8_t character) {
  if (offset < VgaText_PAGE_SIZE) {
    VgaText_buffer[offset] = (uint16_t) character | (uint16_t) attributes << 8;
  }
}

void VgaText_moveCursor(size_t offset) {
  if (offset < VgaText_PAGE_SIZE) {
    Port_out(0x3D4, 0x0F);
    Port_out(0x3D5, offset & 0xFF);

    Port_out(0x3D4, 0x0E);
    Port_out(0x3D5, (offset >> 8) & 0xFF);
  }
}

uint8_t VgaText_color(enum VgaText_Color foreground, enum VgaText_Color background) {
	return foreground | background << 4;
}
