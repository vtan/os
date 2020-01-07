#include "kernel.hpp"

#include "Port.hpp"
#include "String.hpp"
#include "VgaText.hpp"

VgaText::VgaText() {
  const uint8_t cursorStartRegister = 0xA;
  const uint8_t cursorEndRegister = 0xB;
  const uint8_t scanlineStart = 0xA;
  const uint8_t scanlineEnd = 0xF;

  // Enable cursor
  Port_out(0x3D4, cursorStartRegister);
  const uint8_t startLine = (Port_in(0x3D5) & 0xC0) | scanlineStart;
  Port_out(0x3D5, startLine);

  Port_out(0x3D4, cursorEndRegister);
  const uint8_t endLine = (Port_in(0x3D5) & 0xE0) | scanlineEnd;
  Port_out(0x3D5, endLine);
}

void VgaText::put(size_t offset, uint8_t attributes, uint8_t character) {
  if (offset < VgaText::SCREEN_SIZE) {
    this->buffer[offset] = (uint16_t) character | (uint16_t) attributes << 8;
  }
}

void VgaText::scrollUp() {
  memmove(
    (uint8_t*) this->buffer,
    (uint8_t*) (this->buffer + VgaText::WIDTH),
    sizeof(uint16_t) * (VgaText::SCREEN_SIZE - VgaText::WIDTH)
  );

  const uint16_t blank = ' ' | VgaText::colorFrom(VgaText::Color::LIGHT_GREY, VgaText::Color::BLACK) << 8;
  for (size_t i = VgaText::SCREEN_SIZE - VgaText::WIDTH; i < VgaText::SCREEN_SIZE; ++i) {
    this->buffer[i] = blank;
  }
}

void VgaText::moveCursor(size_t offset) {
  if (offset < VgaText::SCREEN_SIZE) {
    Port_out(0x3D4, 0x0F);
    Port_out(0x3D5, offset & 0xFF);

    Port_out(0x3D4, 0x0E);
    Port_out(0x3D5, (offset >> 8) & 0xFF);
  }
}
