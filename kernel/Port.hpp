#pragma once

#include "kernel.hpp"

extern "C" uint8_t port_in8(uint16_t port);
extern "C" void port_out8(uint16_t port, uint8_t value);
