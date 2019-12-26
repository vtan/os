TARGET := i686-elf
AS := $(TARGET)-as
CC := $(TARGET)-gcc

CFLAGS := -std=c11 -ffreestanding -Wall -Wextra
LDFLAGS := -ffreestanding
LDLIBS := -nostdlib -lgcc

KERNEL_BIN := kernel.bin
OBJS := src/boot.o src/kernel.o src/string.o src/Idt.o src/Keyboard.o src/Pic.o src/Port.o src/Terminal.o src/VgaText.o

all: $(KERNEL_BIN)

$(KERNEL_BIN): src/linker.ld  $(OBJS)
	$(CC) -T $< -o $@ $(LDFLAGS) $(LDLIBS) $(OBJS)

clean:
	rm -f $(KERNEL_BIN) $(OBJS)

run: $(KERNEL_BIN)
	qemu-system-x86_64 -no-reboot -kernel $<
