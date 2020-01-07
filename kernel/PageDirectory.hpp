#pragma once

#include "kernel.hpp"

extern "C" uint64_t* kernel_page_table_l4;
extern "C" void PageDirectory_use(uint64_t* physicalL4Table);

struct PageDirectory {
  uint64_t* l4Table;

  inline void use() const { PageDirectory_use((uint64_t*) ((uintptr_t) this->l4Table - KERNEL_MEMORY_OFFSET)); }
};

const PageDirectory KERNEL_PAGE_DIRECTORY =
  PageDirectory{ .l4Table = (uint64_t*) &kernel_page_table_l4 };

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
