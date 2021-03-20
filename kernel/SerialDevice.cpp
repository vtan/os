#include "Port.hpp"
#include "SerialDevice.hpp"

static const uint16_t PORT_COM1 = 0x3F8;

static void writeByte(uint8_t byte);
static bool isTransmitEmpty();

SerialDevice::SerialDevice() {
  // https://wiki.osdev.org/Serial_ports
  port_out8(PORT_COM1 + 1, 0x00);    // Disable all interrupts
  port_out8(PORT_COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
  port_out8(PORT_COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
  port_out8(PORT_COM1 + 1, 0x00);    //                  (hi byte)
  port_out8(PORT_COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
  port_out8(PORT_COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
  port_out8(PORT_COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
  port_out8(PORT_COM1 + 4, 0x1E);    // Set in loopback mode, test the serial chip
  port_out8(PORT_COM1 + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

  if (port_in8(PORT_COM1 + 0) == 0xAE) {
    port_out8(PORT_COM1 + 4, 0x0F);
    initialized = true;
  } else {
    kprintf("Failed to initialize COM1 serial device\n");
  }
}

void SerialDevice::writeString(const char* const str) {
  if (initialized) {
    for (const char* p = str; *p != 0; ++p) {
      writeByte(*p);
    }
  }
}

static void writeByte(uint8_t byte) {
  while (isTransmitEmpty() == 0);
  port_out8(PORT_COM1, byte);
}

static bool isTransmitEmpty() {
  return port_in8(PORT_COM1 + 5) & 0x20;
}
