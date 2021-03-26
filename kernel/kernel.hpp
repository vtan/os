#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define PRIVATE static
#define PACKED __attribute__((__packed__))

#define KBYTES(n) (n * 1024)
#define MBYTES(n) (n * 1024 * 1024)

#define PAGE_SIZE KBYTES(4)

constexpr uint64_t roundUpPow2(uint64_t x, uint64_t n) {
  return (x + n - 1) & (~(n - 1));
}

constexpr uint64_t offsetFromPow2(uint64_t x, uint64_t n) {
  return x & ~(n - 1);
}

// The virtual offset of kernel code and statically allocated memory.
// These addresses are in the -2 GB to 0 space, so they fit in a signed 32-bit pointer
// and we can compile the kernel with -mcmodel=kernel.
#define KERNEL_STATIC_MEMORY_OFFSET 0xFFFF'FFFF'8000'0000ul

// The virtual offset where all of the physical memory is mapped for the kernel.
#define PHYSICAL_MEMORY_OFFSET      0xFFFF'FF80'0000'0000ul

template<typename T>
struct BaseAddress {
  uintptr_t address;

  T operator+(const ptrdiff_t offset) const { return { address + offset }; }
  ptrdiff_t operator-(const T rhs) const { return address - rhs.address; }

  bool operator>=(const T rhs) const { return address >= rhs.address; }
  bool operator<(const T rhs) const { return address < rhs.address; }
} PACKED;

struct UserAddress : public BaseAddress<UserAddress> {} PACKED;

struct PhysicalAddress : public BaseAddress<PhysicalAddress> {
  void* toVirtual() const { return (void*) (address + PHYSICAL_MEMORY_OFFSET); }

  static PhysicalAddress ofVirtual(const void* p) { return { (uintptr_t) p - PHYSICAL_MEMORY_OFFSET }; }
  static PhysicalAddress ofKernelStatic(const void* p) { return { (uintptr_t) p - KERNEL_STATIC_MEMORY_OFFSET }; }
} PACKED;

static_assert(sizeof(UserAddress) == 8);
static_assert(sizeof(PhysicalAddress) == 8);

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
#define klog(format, ...) kprintf(__FILE__ ":" STRINGIFY(__LINE__) ": " format "\n", ##__VA_ARGS__)
#define assert(predicate) if (!(predicate)) { klog("Assertion failed\n"); kernel_halt(); }
