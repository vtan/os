#pragma once

#include "kernel.hpp"

extern "C" void PageDirectory_use(PhysicalAddress);
extern "C" void PageDirectory_flush();
void mapPhysicalMemoryToKernel();

struct PageDirectoryEntry;

struct PageDirectory {
  PageDirectoryEntry* l4Table;

  inline void use() const { PageDirectory_use(PhysicalAddress::ofVirtual(l4Table)); }
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
