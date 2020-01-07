#pragma once

struct Process {
  uintptr_t entryPoint;
  uintptr_t stackTop;
};

class ProcessLoader {
  PageAllocator& pageAllocator;

public:
  ProcessLoader(PageAllocator& pa) : pageAllocator(pa) {}

  void load(void* elf, Process*);
};

extern "C" uint64_t Process_run(struct Process*);
