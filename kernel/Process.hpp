#pragma once

#include "kernel.hpp"

#include "PageAlloc.hpp"
#include "PageDirectory.hpp"

struct Process {
  void* kernelStackPointer;
  const InterruptFrame* continuation;
  PhysicalAddress pageDirectory;
  uint32_t pid;
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
