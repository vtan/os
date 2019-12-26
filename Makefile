TARGET := x86_64-elf
AS := $(TARGET)-as
CC := $(TARGET)-gcc

CFLAGS := -std=c11 -ffreestanding -Wall -Wextra -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2
LDFLAGS := -ffreestanding -z max-page-size=0x1000
LDLIBS := -nostdlib -lgcc

KERNEL_BIN := kernel.bin
ISO := os.iso
OBJS := src/boot.o src/kernel.o src/string.o src/Idt.o src/Keyboard.o src/Pic.o src/Port.o src/Terminal.o src/VgaText.o

all: $(KERNEL_BIN)

$(KERNEL_BIN): src/linker.ld  $(OBJS)
	$(CC) -T $< -o $@ $(LDFLAGS) $(LDLIBS) $(OBJS)

clean:
	rm -rf iso $(ISO) $(KERNEL_BIN) $(OBJS)

$(ISO): $(KERNEL_BIN) grub.cfg
	mkdir -p iso/boot/grub
	cp grub.cfg iso/boot/grub/grub.cfg
	cp $(KERNEL_BIN) iso/boot/$(KERNEL_BIN)
	grub-mkrescue -o $(ISO) iso 2> /dev/null

run: $(ISO)
	qemu-system-x86_64 -no-reboot -cdrom $(ISO)
