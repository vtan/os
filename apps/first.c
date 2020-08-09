#include "userlib.h"

void wait();

uint64_t main() {
  uint64_t counter = 0;
  while (1) {
    print(counter++);
    wait();
  }
  return 0;
}

void wait() {
  uint32_t x = 0x3FFFFFFF;
  while (x-- > 0) {}
}
