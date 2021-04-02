#include "kernel.hpp"

#include "Pic.hpp"
#include "Port.hpp"
#include "Timer.hpp"

namespace Timer {

constexpr uint8_t irqLine = 0;
constexpr uint16_t reload = 0x2E9C; // Almost 100 Hz frequency

namespace Ports {
constexpr uint16_t channel1 = 0x40;
constexpr uint16_t modeRegister = 0x43;
}

void initialize() {
  port_out8(Ports::modeRegister, 0b00'11'011'0); // Set channel 1 to square wave
  port_out8(Ports::channel1, reload & 0xFF);
  port_out8(Ports::channel1, (reload >> 8) & 0xFF);
  Pic::enableInterrupt(irqLine);
}

}
