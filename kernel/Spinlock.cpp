#include "Spinlock.hpp"

extern "C" uint8_t atomicSwap(volatile uint8_t*, uint8_t);

bool Spinlock::tryAcquire() {
  const bool previousState = atomicSwap(&lock, true);
  return !previousState;
}

void Spinlock::acquire() {
  while (atomicSwap(&lock, true)) {}
}

void Spinlock::release() {
  lock = false;
}
