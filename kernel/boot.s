.intel_syntax noprefix

.set KERNEL_MEMORY_OFFSET, 0xFFFFFF8000000000

.section .multiboot

.align 4
.4byte 0xe85250d6                # magic number (multiboot 2)
.4byte 0                         # architecture 0 (protected mode i386)
.4byte 96                        # header length
# checksum
.4byte 0x100000000 - (0xe85250d6 + 0 + 96)
.2byte 0    # type
.2byte 0    # flags
.4byte 8    # size

.section .data

gdt:
  gdt_null_segment = . - gdt
  .2byte 0xFFFF                    # Limit (low).
  .2byte 0                         # Base (low).
  .byte 0                         # Base (middle)
  .byte 0                         # Access.
  .byte 1                         # Granularity.
  .byte 0                         # Base (high).

  gdt_code_segment = . - gdt
  .2byte 0                         # Limit (low).
  .2byte 0                         # Base (low).
  .byte 0                         # Base (middle)
  .byte 0b10011010                 # Access (exec/read).
  .byte 0b10101111                 # Granularity, 64 bits flag, limit19:16.
  .byte 0                         # Base (high).

  gdt_data_segment = . - gdt
  .2byte 0                         # Limit (low).
  .2byte 0                         # Base (low).
  .byte 0                         # Base (middle)
  .byte 0b10010010                 # Access (read/write).
  .byte 0b00000000                 # Granularity.
  .byte 0                         # Base (high).

gdt_pointer:
  .2byte (. - gdt - 1)             # Limit.
  .8byte offset gdt                     # Base.

.section .bss

.align 16
stack_bottom:
  .skip 16384 # 16 KiB
stack_top:

.align 4096
.global kernel_page_table_l4
kernel_page_table_l4:
  .skip 4096
.global kernel_page_table_l3
kernel_page_table_l3:
  .skip 4096
kernel_page_table_l2:
  .skip 4096
kernel_page_table_l1:
  .skip 4096

.section .text

.code32
.global _start
.type _start, @function
_start:
  # The kernel will live at 0+2MB in the physical address space
  # and at 0xFFFFFF8000000000+2MB in the virtual address space.
  # We set up two virtual address ranges in the page table hierarchy
  # pointing to this physical range:
  # - L4 entry 511, mapping the virtual space to the physical,
  # - L4 entry 0, mapping the physical space to itself temporarily,
  #   so we don't get a page fault while we're executing code here.
  # Both entries point to L3[0], which points to L2[0], which is a large (2MB) page itself.

  mov eax, offset (kernel_page_table_l3 - KERNEL_MEMORY_OFFSET)
  or eax, 0x03
  mov dword ptr [kernel_page_table_l4 - KERNEL_MEMORY_OFFSET], eax           # Link to L3 from 0 for identity page
  mov dword ptr [kernel_page_table_l4 - KERNEL_MEMORY_OFFSET + 8 * 511], eax # Link to L3 from FFFFFF8000000000

  mov eax, offset (kernel_page_table_l2 - KERNEL_MEMORY_OFFSET)
  or eax, 0x03
  mov dword ptr [kernel_page_table_l3 - KERNEL_MEMORY_OFFSET], eax

  mov dword ptr [kernel_page_table_l2 - KERNEL_MEMORY_OFFSET], 0x83 # 2MB page

  mov edi, offset (kernel_page_table_l4 - KERNEL_MEMORY_OFFSET)
  mov cr3, edi

  # Enable PAE and PSE
  mov eax, cr4
  or eax, 0b00110000
  mov cr4, eax

  mov ecx, 0xC0000080          # Set the C-register to 0xC0000080, which is the EFER MSR.
  rdmsr                        # Read from the model-specific register.
  or eax, 0x100                # Set the LM-bit which is the 9th bit (bit 8).
  wrmsr                        # Write to the model-specific register.

  # Enable paging
  mov eax, cr0
  or eax, 1 << 31
  mov cr0, eax

  # Using physical address of GDTR because we're still in 32-bit mode.
  # We'll need to switch to the 64-bit virtual address before disabling the identity page.
  lgdt [gdt_pointer - KERNEL_MEMORY_OFFSET]
  jmp gdt_code_segment:(_start64_physical - KERNEL_MEMORY_OFFSET)

.code64
.global _start64_physical
.type _start64_physical, @function
_start64_physical:
  # We're still executing code from the physical address range,
  # let's jump to the virtual address.
  movabs rax, offset _start64
  jmp rax

.global _start64
.type _start64, @function
_start64:
  mov ax, gdt_data_segment
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  # Disable identity page, flush TLB
  mov qword ptr [kernel_page_table_l4 - KERNEL_MEMORY_OFFSET], 0
  mov rax, cr3
  mov cr3, rax

  movabs rsp, offset stack_top

  movabs rax, offset gdt_pointer
  lgdt [rax]

  call idt_fill
  movabs rax, offset idt_pointer
  lidt [rax]
  sti

  # The boot loader put the pointer to the multiboot2 information structure
  # to rbx and we haven't changed it.
  mov rdi, KERNEL_MEMORY_OFFSET
  add rdi, rbx

  call kernel_main

  cli
.global halt
halt:
  hlt
  jmp halt
