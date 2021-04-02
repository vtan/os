#include "ProcessExecutor.hpp"
#include "Process.hpp"

#define MAX_PROCESSES 4

extern "C" void switchToProcess(Process*);

Process* runningProcess = nullptr;

namespace ProcessExecutor {

Process processes[MAX_PROCESSES] = { {0, 0, 0, 0, 0} };

static Process* findNextRunnable(Process* process) {
  const Process* end = processes + MAX_PROCESSES;
  while (process < end) {
    if (process->runnable) {
      return process;
    }
    ++process;
  }
  return nullptr;
}

Process* allocate() {
  for (int i = 0; i < MAX_PROCESSES; ++i) {
    Process* p = &processes[i];
    if (!p->runnable) {
      p->pid = i;
      return p;
    }
  }
  return nullptr;
}

void switchToNext() {
  Process* process = runningProcess;
  if (process) {
    process->runnable = false;
    ++process;
  } else {
    process = &processes[0];
  }
  process = findNextRunnable(process);

  if (process) {
    kprintf("Running process %d\n", process - processes);
    switchToProcess(process);
  }
}

}
