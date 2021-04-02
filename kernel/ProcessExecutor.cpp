#include "kernel.hpp"
#include "Pic.hpp"
#include "Process.hpp"
#include "ProcessExecutor.hpp"
#include "Spinlock.hpp"

#define MAX_PROCESSES 16

extern "C" [[noreturn]] void switchToProcess(Process*);

Process* runningProcess = nullptr;
Spinlock processSwitchLock;

namespace ProcessExecutor {

[[noreturn]] static void switchToNext();

Process processes[MAX_PROCESSES] = { {0, 0, 0, 0, 0} };

static Process* findNextRunnable(Process* ref) {
  if (ref == nullptr) {
    ref = processes;
  }
  const Process* end = processes + MAX_PROCESSES;

  for (Process* p = ref + 1; p < end; ++p) {
    if (p->runnable) {
      return p;
    }
  }
  for (Process* p = processes; p <= ref; ++p) {
    if (p->runnable) {
      return p;
    }
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

void scheduleNextProcess(const InterruptFrame* continuation) {
  const bool acquiredLock = processSwitchLock.tryAcquire();
  Pic::acknowledgeInterrupt();
  if (acquiredLock) {
    if (runningProcess != nullptr) {
      runningProcess->continuation = continuation;
    }
    switchToNext();
  }
}

[[noreturn]] void exitCurrentProcess() {
  runningProcess->runnable = false;
  if (processSwitchLock.tryAcquire()) {
    switchToNext();
  } else {
    // This process will never get scheduled again, so we're waiting until we get interrupted.
    while (true) { asm("hlt"); }
  }
}

[[noreturn]] static void switchToNext() {
  Process* nextProcess = findNextRunnable(runningProcess);
  if (nextProcess) {
    disableInterrupts();
    processSwitchLock.release();
    runningProcess = nextProcess;
    switchToProcess(nextProcess);
  } else {
    kprintf("No more processes to run, halting.\n");
    kernel_halt();
  }
}

}
