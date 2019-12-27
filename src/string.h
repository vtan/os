#pragma once

#include <stddef.h>
#include <stdint.h>

void String_uformat(uint64_t number, char* str, int radix);

void memset(uint8_t* dest, uint8_t byte, size_t count);
void memmove(uint8_t* dest, const uint8_t* src, size_t count);
