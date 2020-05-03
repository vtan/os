TARGET := x86_64-elf
AS := $(TARGET)-as
CC := $(TARGET)-gcc
CXX := $(TARGET)-gcc

CFLAGS := -std=c11 -ffreestanding -Wall -Wextra -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2
CXXFLAGS := -std=c++11 -ffreestanding -fno-exceptions -fno-rtti -Wall -Wextra -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2
LDFLAGS := -ffreestanding -z max-page-size=0x1000
LDLIBS := -nostdlib -lgcc

KERNEL_BIN := kernel.bin
ISO := os.iso
OBJS := kernel/boot.o kernel/kernel.o kernel/Interrupt_asm.o \
	kernel/Keyboard.o kernel/PageAlloc.o kernel/PageDirectory.o kernel/PageDirectory_asm.o \
	kernel/Pic.o kernel/Port.o kernel/Process.o kernel/Process_asm.o kernel/String.o kernel/Terminal.o kernel/VgaText.o

CRTI_OBJ := kernel/crti.o
CRTBEGIN_OBJ := $(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ := $(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
CRTN_OBJ := kernel/crtn.o

OBJ_LINK_LIST := $(CRTI_OBJ) $(CRTBEGIN_OBJ) $(OBJS) $(CRTEND_OBJ) $(CRTN_OBJ)
ALL_OBJS := $(CRTI_OBJ) $(OBJS) $(CRTN_OBJ)

APPS := apps/first.elf

all: $(KERNEL_BIN) $(APPS)

iso: $(ISO)

$(KERNEL_BIN): kernel/linker.ld  $(OBJ_LINK_LIST)
	$(CC) -T $< -o $@ $(LDFLAGS) $(LDLIBS) $(OBJ_LINK_LIST)

apps/%.elf: apps/%.o apps/userlib.o
	$(CC) -o $@ $(LDFLAGS) $(LDLIBS) $^

clean:
	rm -rf iso $(ISO) $(KERNEL_BIN) $(ALL_OBJS) $(APPS) apps/userlib.o

$(ISO): $(KERNEL_BIN) $(APPS) grub.cfg
	mkdir -p iso/boot/grub
	cp grub.cfg iso/boot/grub/grub.cfg
	cp $(KERNEL_BIN) iso/boot/$(KERNEL_BIN)
	cp apps/first.elf iso/boot/initrd
	grub-mkrescue -o $(ISO) iso 2> /dev/null

QEMU_ARGS := $(QEMU_ARGS) -no-reboot -cdrom $(ISO)

run: $(ISO)
	qemu-system-x86_64 $(QEMU_ARGS)

run-bochs: $(ISO)
	bochs -q
