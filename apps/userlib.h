#pragma once

#include <stdint.h>

extern uint64_t syscall(uint64_t syscallNumber, uint64_t arg);

inline static void print(uint64_t n) {
  syscall(0x100, n);
}
