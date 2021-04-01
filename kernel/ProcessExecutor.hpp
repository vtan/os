#pragma once

#include "kernel.hpp"
#include "Process.hpp"

extern Process* runningProcess;
extern "C" void processRun(Process*);

namespace ProcessExecutor {
  Process* allocate();
  void switchToNext();
}
