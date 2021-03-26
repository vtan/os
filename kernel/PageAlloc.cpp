#include "kernel.hpp"

#include "PageAlloc.hpp"

PhysicalAddress PageAllocator::allocate() {
  PhysicalAddress page = nextPage;
  nextPage = nextPage + PAGE_SIZE;
  return page;
}
