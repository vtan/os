#pragma once

#include "kernel.hpp"

extern "C" uint8_t Port_in(uint16_t port);
extern "C" void Port_out(uint16_t port, uint8_t value);
