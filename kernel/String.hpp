#pragma once

#include "kernel.hpp"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

void memset(void* dest, int byte, size_t count);
void memmove(void* dest, const void* src, size_t count);
void memcpy(void* dest, const void* source, size_t count);

class String {
private:
  static constexpr const char* const digits = "0123456789ABCDEF";

public:
  template <typename PutChar>
  static void vformat(PutChar putChar, const char* format, va_list args) {
    bool percentMode = false;
    while (*format != 0) {
      if (percentMode) {
        switch (*format) {
        case 'd':
          String::formatUnsigned(putChar, va_arg(args, uint64_t), 10, 0, 0);
          break;
        case 'x':
          String::formatUnsigned(putChar, va_arg(args, uint64_t), 16, 0, 0);
          break;
        case 'p':
          putChar('0');
          putChar('x');
          String::formatUnsigned(putChar, va_arg(args, uint64_t), 16, 4, '\'');
          break;
        case 's': {
          for (const char* p = va_arg(args, const char*); *p != 0; ++p) {
            putChar(*p);
          }
          break;
        }
        default:
          putChar(*format);
        }
        percentMode = false;
      } else if (*format == '%') {
        percentMode = true;
      } else {
        putChar(*format);
      }
      ++format;
    }
  }

  template <typename PutChar>
  static void format(PutChar putChar, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vformat(putChar, format, args);
    va_end(args);
  }

  template <typename PutChar>
  static void formatUnsigned(PutChar putChar, uint64_t number, const int radix, const int digitsPerSeparator, const char separator) {
    if (radix < 0 || radix > 16) {
      return;
    }

    char buffer[64];
    char* p = buffer;
    int digitsSinceSeparator = 1;
    do {
      uint64_t next = number / radix;
      uint64_t currentDigit = number - next * radix;
      *(p++) = digits[currentDigit];
      number = next;

      if (digitsPerSeparator > 0 && number > 0) {
        if (digitsSinceSeparator == digitsPerSeparator) {
          *(p++) = separator;
          digitsSinceSeparator = 1;
        } else {
          ++digitsSinceSeparator;
        }
      }
    } while (number > 0);

    --p;
    while (p >= buffer) {
      putChar(*p--);
    }
  }
};
