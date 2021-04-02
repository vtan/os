#pragma once

#include "kernel.hpp"
#include "Process.hpp"

extern "C" Process* runningProcess;

namespace ProcessExecutor {
  Process* allocate();
  void switchToNext();
}
