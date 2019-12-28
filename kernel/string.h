#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

char* String_uformat(uint64_t number, char* str, int radix);
void String_printf(char* output, const char* format, ...);
void String_vprintf(char* output, const char* format, va_list);

void memset(uint8_t* dest, uint8_t byte, size_t count);
void memmove(uint8_t* dest, const uint8_t* src, size_t count);
