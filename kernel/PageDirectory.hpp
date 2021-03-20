#pragma once

#include "kernel.hpp"

extern "C" void PageDirectory_use(uint64_t* physicalL4Table);
extern "C" void PageDirectory_flush();
void mapPhysicalMemoryToKernel();

struct PageDirectory {
  uint64_t* l4Table;

  inline void use() const { PageDirectory_use((uint64_t*) ((uintptr_t) this->l4Table - KERNEL_STATIC_MEMORY_OFFSET)); }
};

class PageAllocator;

class PageDirectoryManager {
  PageAllocator& pageAllocator;

public:
  PageDirectoryManager(PageAllocator& pa) : pageAllocator(pa) {}

  PageDirectory create();
  void addMapping(uintptr_t virtualPageBase, uintptr_t physicalPageBase, PageDirectory&);
private:
  uint64_t* getOrAllocSubtable(uint64_t* table, size_t index);
};
