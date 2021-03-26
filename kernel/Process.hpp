#pragma once

#include "kernel.hpp"

#include "PageAlloc.hpp"
#include "PageDirectory.hpp"

struct Process {
  void* kernelStackPointer;
  UserAddress entryPoint;
  UserAddress userStackPointer;
  PageDirectory pageDirectory;
  bool runnable;
};

class ProcessLoader {
  PageAllocator& pageAllocator;
  PageDirectoryManager& pageDirectoryManager;

public:
  ProcessLoader(PageAllocator& pa, PageDirectoryManager& pdm)
    : pageAllocator(pa), pageDirectoryManager(pdm) {}

  void load(void* elf, Process*);
};
