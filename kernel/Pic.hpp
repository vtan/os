#pragma once

#include "kernel.hpp"

namespace Pic {

void initialize();

constexpr uint8_t MASTER_COMMAND = 0x20;
constexpr uint8_t MASTER_DATA = 0x21;
constexpr uint8_t SLAVE_COMMAND = 0xA0;
constexpr uint8_t SLAVE_DATA = 0xA1;

constexpr uint8_t END_OF_INTERRUPT = 0x20;

}
