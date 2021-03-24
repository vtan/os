#pragma once

#include "kernel.hpp"

#include "PageAlloc.hpp"
#include "PageDirectory.hpp"

struct Process {
  uintptr_t kernelStackPointer;
  uintptr_t entryPoint;
  uintptr_t userStackPointer;
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
