#include "kernel.hpp"
#include "String.hpp"

void memset(void* dest, int byte, size_t count) {
  uint8_t* start = (uint8_t*) dest;
  const uint8_t* end = start + count;
  for (uint8_t* p = start; p < end; ++p) {
    *p = byte;
  }
}

void memmove(void* destRaw, const void* srcRaw, size_t count) {
  auto dest = (uint8_t*) destRaw;
  auto src = (const uint8_t*) srcRaw;
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

void memcpy(void* destRaw, const void* sourceRaw, size_t count) {
  auto dest = (uint8_t*) destRaw;
  auto source = (const uint8_t*) sourceRaw;
  const uint8_t* end = source + count;
  while (source < end) {
    *dest++ = *source++;
  }
}
