#pragma once

struct Process {
  uintptr_t entryPoint;
  uintptr_t stackTop;
};

void Process_load(void* elf, struct Process*);
extern "C" uint64_t Process_run(struct Process*);
