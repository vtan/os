#include "kernel.hpp"

#include "PageAlloc.hpp"

void* PageAllocator::allocate() {
  void* page = this->nextPage;
  this->nextPage += PAGE_SIZE;
  return page;
}
