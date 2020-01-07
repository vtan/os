#pragma once

#include "kernel.hpp"

class PageAllocator;

extern "C" void PageDirectory_use(uint64_t* physicalL4Table);

class PageDirectory {
  PageAllocator& pageAllocator;
  uint64_t* l4Table;

public:
  PageDirectory(PageAllocator&);

  void map(uintptr_t virtualPageBase, uintptr_t physicalPageBase);

  inline void use() { PageDirectory_use((uint64_t*) (((uintptr_t) this->l4Table) - KERNEL_MEMORY_OFFSET)); }
private:
  uint64_t* getOrAllocSubtable(uint64_t* table, size_t index);
};
