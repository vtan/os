export TARGET := x86_64-elf
export AS := nasm

KERNEL_BIN := kernel/kernel.bin
ISO := os.iso

all: kernel apps

.PHONY: kernel
kernel:
	$(MAKE) -C kernel

.PHONY: apps
apps:
	$(MAKE) -C apps

iso: $(ISO)

clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C apps clean
	rm -rf iso $(ISO)

$(ISO): kernel apps grub.cfg
	mkdir -p iso/boot/grub
	cp grub.cfg iso/boot/grub/grub.cfg
	cp $(KERNEL_BIN) iso/boot/kernel.bin
	cp apps/first.elf iso/boot/initrd
	touch iso/boot/initrd
	grub-mkrescue -o $(ISO) iso 2> /dev/null

QEMU_ARGS := $(QEMU_ARGS) -serial stdio -no-reboot -cdrom $(ISO)

run: $(ISO)
	qemu-system-x86_64 $(QEMU_ARGS)

run-bochs: $(ISO)
	bochs -q
