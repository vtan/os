#pragma once

#include "kernel.hpp"
#include "Process.hpp"

extern Process* runningProcess;
extern "C" void Process_run(Process*);

namespace ProcessExecutor {

  Process* allocate();
  void switchToNext();
}
