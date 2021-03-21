CC := ../toolchain/gcc/bin/$(TARGET)-gcc
CFLAGS := -std=c11 -ffreestanding -Wall -Wextra -mno-red-zone -mno-mmx -mno-sse -mno-sse2
ASFLAGS := -f elf64
LDFLAGS := -ffreestanding -z max-page-size=0x1000
LDLIBS := -nostdlib -lgcc

APPS := a.elf first.elf

all: apps.tar

apps.tar: $(APPS)
	tar -c -f $@ $^

%.elf: %.o userlib.o
	$(CC) -o $@ $(LDFLAGS) $(LDLIBS) $^

clean:
	rm -rf $(APPS) userlib.s.o apps.tar
