#pragma once

#define Pic_MASTER_COMMAND 0x20
#define Pic_MASTER_DATA    0x21
#define Pic_SLAVE_COMMAND  0xA0
#define Pic_SLAVE_DATA     0xA1

#define Pic_END_OF_INTERRUPT 0x20

void Pic_init();
