#pragma once

#include <stdint.h>

#define KERNEL_MEMORY_OFFSET 0xFFFFFF8000000000ul

struct Kernel_InterruptStack {
  uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
  uint64_t rdi, rsi, rbp, rbx, rdx, rcx, rax;
  uint64_t interruptNumber, errorCode;
  uint64_t rip, cs, rflags, rsp, ss;
};
