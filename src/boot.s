.intel_syntax noprefix

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
page_map_l4_table:
  .skip 4096
page_dictionary_pointer_table:
  .skip 4096
page_directory_table:
  .skip 4096
page_table:
  .skip 4096

.section .text

.code32
.global _start
.type _start, @function
_start:
  mov eax, offset page_dictionary_pointer_table
  or eax, 0x03
  mov dword ptr [page_map_l4_table], eax

  mov eax, offset page_directory_table
  or eax, 0x03
  mov dword ptr [page_dictionary_pointer_table], eax

  mov dword ptr [page_directory_table], 0x83 # 2MB identity page

  mov edi, offset page_map_l4_table
  mov cr3, edi

  # Enable PAE and PSE
  mov eax, cr4
  or eax, 0b00110000
  mov cr4, eax

  mov ecx, 0xC0000080          # Set the C-register to 0xC0000080, which is the EFER MSR.
  rdmsr                        # Read from the model-specific register.
  or eax, 0x100                # Set the LM-bit which is the 9th bit (bit 8).
  wrmsr                        # Write to the model-specific register.

  # Enable paging and write protection
  mov eax, cr0
  or eax, 1 << 31 | 1 << 16
  mov cr0, eax

  lgdt [gdt_pointer]
  jmp gdt_code_segment:_start64

.code64
.global _start64
.type _start64, @function
_start64:
  mov ax, gdt_data_segment
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  mov rax, 0x2f592f412f4b2f4f
  mov qword [0xb8000], rax

  # mov esp, offset stack_top

  # call Idt_init
  # call Idt_load
  # sti

  # call kernel_main

  cli
halt:
  hlt
  jmp halt

/*
.global interrupt_handler
interrupt_handler:
  pushad
  cld
  # call Keyboard_handler
  popad
  iretd

.global Keyboard_interrupt
Keyboard_interrupt:
  pushad
  cld
  call Keyboard_handler
  popad
  iretd

Idt_load:
  lidt [Idt_pointer]
  ret
*/
