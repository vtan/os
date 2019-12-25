#include "string.h"

void memset(uint8_t* dest, uint8_t byte, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    dest[i] = byte;
  }
}

void memmove(uint8_t* dest, const uint8_t* src, size_t count) {
  if (src > dest) {
    for (size_t i = 0; i < count; ++i) {
      dest[i] = src[i];
    }
  } else if (src < dest) {
    for (size_t i = count; i > 0; --i) {
      dest[i - 1] = src[i - 1];
    }
  }
}
