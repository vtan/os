#include "kernel.hpp"
#include "PageAlloc.hpp"

struct PageAlloc_State {
  void* nextPage;
};

PRIVATE struct PageAlloc_State state;

void PageAlloc_init(void* memoryBase) {
  state.nextPage = memoryBase;
}

void* PageAlloc_alloc() {
  void* page = state.nextPage;
  state.nextPage += PAGE_SIZE;
  return page;
}
