#pragma once

#include "kernel.hpp"

extern "C" void pageDirectoryUse(PhysicalAddress);
extern "C" void pageDirectoryFlush();
void mapPhysicalMemoryToKernel();

struct PageDirectoryEntry;

struct PageDirectory {
  PageDirectoryEntry* l4Table;

  inline void use() const { pageDirectoryUse(PhysicalAddress::ofVirtual(l4Table)); }
};

class PageAllocator;

class PageDirectoryManager {
  PageAllocator& pageAllocator;

public:
  PageDirectoryManager(PageAllocator& pa) : pageAllocator(pa) {}

  PageDirectory create();
  void addMapping(UserAddress, PhysicalAddress, PageDirectory&);
private:
  PageDirectoryEntry* getOrAllocSubtable(PageDirectoryEntry* table, size_t index);
};
