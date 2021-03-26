#pragma once

#include "kernel.hpp"

class PageAllocator {
  PhysicalAddress nextPage;

public:
  PageAllocator(PhysicalAddress start) : nextPage(start) {}

  PhysicalAddress allocate();
};
