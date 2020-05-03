#pragma once

struct Process {
  uintptr_t entryPoint;
  uintptr_t userStackPointer;
  uintptr_t kernelStackPointer;
};

class ProcessLoader {
  PageAllocator& pageAllocator;
  PageDirectoryManager& pageDirectoryManager;

public:
  ProcessLoader(PageAllocator& pa, PageDirectoryManager& pdm)
    : pageAllocator(pa), pageDirectoryManager(pdm) {}

  void load(void* elf, Process*);
};

extern "C" uint64_t Process_run(struct Process*);
