#include <stdarg.h>

#include "kernel.hpp"
#include "String.hpp"

const char* const digits = "0123456789ABCDEF";

char* String_uformat(uint64_t number, char* str, const int radix, const int digitsPerSeparator, const char separator) {
  if (radix < 0 || radix > 16) {
    str[0] = 0;
    return 0;
  }

  char* p = str;
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

  char* end = p--;
  *end = 0;

  while (str < p) {
    char tmp = *str;
    *str = *p;
    *p = tmp;
    ++str;
    --p;
  }

  return end;
}

void String_printf(char* output, const char* format, ...) {
  va_list args;
  va_start(args, format);
  String_vprintf(output, format, args);
  va_end(args);
}

void String_vprintf(char* output, const char* format, va_list args) {
  bool percentMode = false;
  while (*format != 0) {
    if (percentMode) {
      switch (*format) {
      case 'd':
        output = String_uformat(va_arg(args, uint64_t), output, 10, 0, 0);
        break;
      case 'x':
        output = String_uformat(va_arg(args, uint64_t), output, 16, 0, 0);
        break;
      case 'p':
        *output++ = '0';
        *output++ = 'x';
        output = String_uformat(va_arg(args, uint64_t), output, 16, 4, '_');
        break;
      case 's': {
        const char* p = va_arg(args, const char*);
        for (;;) {
          *output = *p;
          if (*p == 0) {
            break;
          }
          ++output;
          ++p;
        }
        break;
      }
      default:
        *output++ = *format;
      }
      percentMode = false;
    } else if (*format == '%') {
      percentMode = true;
    } else {
      *output++ = *format;
    }
    ++format;
  }
  *output = 0;
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

void memcpy(uint8_t* dest, const uint8_t* source, size_t count) {
  const uint8_t* end = source + count;
  while (source < end) {
    *dest++ = *source++;
  }
}
