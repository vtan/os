#pragma once

#include "kernel.hpp"

class SerialDevice {
private:
  bool initialized = false;
public:
  SerialDevice();

  void writeString(const char*);
};
