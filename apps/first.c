#include <stdint.h>

extern uint64_t syscall(uint64_t syscallNumber, uint64_t arg);

void wait();

uint64_t main() {
  uint64_t counter = 0;
  while (1) {
    syscall(0x100, counter++);
    wait();
  }
  return 0;
}

void wait() {
  uint32_t x = 0x3FFFFFFF;
  while (x-- > 0) {}
}
