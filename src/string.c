#include "string.h"

const char* const digits = "0123456789ABCDEF";

void String_uformat(uint64_t number, char* str, const int radix) {
  if (radix < 0 || radix > 16) {
    str[0] = 0;
    return;
  }

  char* p = str;
  do {
    uint64_t next = number / radix;
    uint64_t currentDigit = number - next * radix;
    *(p++) = digits[currentDigit];
    number = next;
  } while (number > 0);

  *(p--) = 0;

  while (str < p) {
    char tmp = *str;
    *str = *p;
    *p = tmp;
    ++str;
    --p;
  }
}

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
