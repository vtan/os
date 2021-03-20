CC := ../toolchain/gcc/bin/$(TARGET)-gcc
CXXFLAGS := -std=c++17 -ffreestanding -fno-exceptions -fno-rtti -Wall -Wextra -mcmodel=kernel -mno-red-zone -mno-mmx -mno-sse -mno-sse2
ASFLAGS := -f elf64
LDFLAGS := -ffreestanding -z max-page-size=0x1000
LDLIBS := -nostdlib -lgcc

KERNEL_BIN := kernel.bin
CXXFILES := $(wildcard *.cpp)
SFILES := $(wildcard *.s)
OBJS := $(addsuffix .o, $(CXXFILES)) $(addsuffix .o, $(SFILES))

CRTI_OBJ := crti.s.o
CRTBEGIN_OBJ := $(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ := $(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
CRTN_OBJ := crtn.s.o

OBJ_LINK_LIST := $(CRTI_OBJ) $(CRTBEGIN_OBJ) $(filter-out $(CRTI_OBJ) $(CRTN_OBJ), $(OBJS)) $(CRTEND_OBJ) $(CRTN_OBJ)

all: $(KERNEL_BIN)

$(KERNEL_BIN): linker.ld  $(OBJ_LINK_LIST)
	$(CC) -T $< -o $@ $(LDFLAGS) $(LDLIBS) $(OBJ_LINK_LIST)

%.cpp.o: %.cpp
	$(CC) $(CXXFLAGS) -MMD -c -o $@ $<

%.s.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<

clean:
	rm -rf iso $(KERNEL_BIN) $(OBJS) $(addsuffix .d, $(CXXFILES) $(SFILES))

-include *.d
