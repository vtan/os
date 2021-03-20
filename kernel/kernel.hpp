#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define PRIVATE static

#define KBYTES(n) (n * 1024)
#define MBYTES(n) (n * 1024 * 1024)

#define PAGE_SIZE KBYTES(4)

#define KERNEL_MEMORY_OFFSET 0xFFFFFFFF80000000ul

struct InterruptFrame {
  uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
  uint64_t rdi, rsi, rbp, rbx, rdx, rcx, rax;
  uint64_t interruptNumber, errorCode;
  uint64_t rip, cs, rflags, rsp, ss;
};

struct SyscallFrame {
  uint64_t r15, r14, r13, r12, rsi, rdi, rbp, rbx;
  uint64_t syscallArg, syscallNumber;
  uint64_t rip, rflags, rsp;
};

void kprintf(const char* format, ...);

extern "C" void kernel_halt();

#define STRINGIFY_DETAIL(x) #x
#define STRINGIFY(x) STRINGIFY_DETAIL(x)
#define klog(format, ...) kprintf(__FILE__ ":" STRINGIFY(__LINE__) ":" format "\n", ##__VA_ARGS__)
