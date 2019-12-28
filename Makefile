TARGET := x86_64-elf
AS := $(TARGET)-as
CC := $(TARGET)-gcc

CFLAGS := -std=c11 -ffreestanding -Wall -Wextra -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2
LDFLAGS := -ffreestanding -z max-page-size=0x1000
LDLIBS := -nostdlib -lgcc

KERNEL_BIN := kernel.bin
ISO := os.iso
OBJS := kernel/boot.o kernel/kernel.o kernel/string.o kernel/Interrupt.o kernel/Interrupt_asm.o \
	kernel/Keyboard.o kernel/Pic.o kernel/Port.o kernel/Terminal.o kernel/VgaText.o

APPS := apps/first.elf

all: $(KERNEL_BIN)

$(KERNEL_BIN): kernel/linker.ld  $(OBJS)
	$(CC) -T $< -o $@ $(LDFLAGS) $(LDLIBS) $(OBJS)

apps/%.elf: apps/%.o
	$(CC) -o $@ $(LDFLAGS) $(LDLIBS) $<

clean:
	rm -rf iso $(ISO) $(KERNEL_BIN) $(OBJS) $(APPS)

$(ISO): $(KERNEL_BIN) grub.cfg
	mkdir -p iso/boot/grub
	cp grub.cfg iso/boot/grub/grub.cfg
	cp $(KERNEL_BIN) iso/boot/$(KERNEL_BIN)
	grub-mkrescue -o $(ISO) iso 2> /dev/null

QEMU_ARGS := $(QEMU_ARGS) -no-reboot -cdrom $(ISO)

run: $(ISO)
	qemu-system-x86_64 $(QEMU_ARGS)
