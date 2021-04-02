#pragma once

#include "kernel.hpp"

class Spinlock {
private:
  volatile uint8_t lock = 0;

public:
  bool tryAcquire();
  void acquire();
  void release();
};
