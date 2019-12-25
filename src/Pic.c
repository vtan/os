#include "Pic.h"
#include "Port.h"

void Pic_init() {
  Port_out(Pic_MASTER_COMMAND, 0x10);
  Port_out(Pic_SLAVE_COMMAND, 0x10);

  Port_out(Pic_MASTER_DATA, 0x20);
  Port_out(Pic_SLAVE_DATA, 0x28);

  Port_out(Pic_MASTER_DATA, 0x00);
  Port_out(Pic_SLAVE_DATA, 0x00);

  Port_out(Pic_MASTER_DATA, 0x01);
  Port_out(Pic_SLAVE_DATA, 0x01);

  Port_out(Pic_MASTER_DATA, 0xFF);
  Port_out(Pic_SLAVE_DATA, 0xFF);
}
