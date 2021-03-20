#include "Pic.hpp"
#include "Port.hpp"

void Pic_init() {
  port_out8(Pic_MASTER_COMMAND, 0x10);
  port_out8(Pic_SLAVE_COMMAND, 0x10);

  port_out8(Pic_MASTER_DATA, 0x20);
  port_out8(Pic_SLAVE_DATA, 0x28);

  port_out8(Pic_MASTER_DATA, 0x00);
  port_out8(Pic_SLAVE_DATA, 0x00);

  port_out8(Pic_MASTER_DATA, 0x01);
  port_out8(Pic_SLAVE_DATA, 0x01);

  port_out8(Pic_MASTER_DATA, 0xFF);
  port_out8(Pic_SLAVE_DATA, 0xFF);
}
