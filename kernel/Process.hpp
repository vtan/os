#pragma once

struct Process {
  uintptr_t kernelStackPointer;
  uintptr_t entryPoint;
  uintptr_t userStackPointer;
};

inline Process* runningProcess;

class ProcessLoader {
  PageAllocator& pageAllocator;
  PageDirectoryManager& pageDirectoryManager;

public:
  ProcessLoader(PageAllocator& pa, PageDirectoryManager& pdm)
    : pageAllocator(pa), pageDirectoryManager(pdm) {}

  void load(void* elf, Process*);
};

extern "C" void Process_run(Process*);