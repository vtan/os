#include "Pic.hpp"
#include "Port.hpp"

namespace Pic {

void initialize() {
  port_out8(Pic::MASTER_COMMAND, 0x10);
  port_out8(Pic::SLAVE_COMMAND, 0x10);

  port_out8(Pic::MASTER_DATA, 0x20);
  port_out8(Pic::SLAVE_DATA, 0x28);

  port_out8(Pic::MASTER_DATA, 0x00);
  port_out8(Pic::SLAVE_DATA, 0x00);

  port_out8(Pic::MASTER_DATA, 0x01);
  port_out8(Pic::SLAVE_DATA, 0x01);

  port_out8(Pic::MASTER_DATA, 0xFF);
  port_out8(Pic::SLAVE_DATA, 0xFF);
}

}
