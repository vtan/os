KERNEL_VIRTUAL_OFFSET equ 0xFFFFFFFF80000000

STAR equ 0xC0000081
LSTAR equ 0xC0000082

section .multiboot
align 4
  dd 0xe85250d6                ; magic number (multiboot 2)
  dd 0                         ; architecture 0 (protected mode i386)
  dd 96                        ; header length
  dd 0x100000000 - (0xe85250d6 + 0 + 96)  ; checksum
  dw 0    ; type
  dw 0    ; flags
  dd 8    ; size

section .data

gdt:
  GDT_NULL_SEGMENT equ $ - gdt
  dw 0xFFFF                    ; Limit (low).
  dw 0                         ; Base (low).
  db 0                         ; Base (middle)
  db 0                         ; Access.
  db 1                         ; Granularity.
  db 0                         ; Base (high).

  GDT_CODE_SEGMENT equ $ - gdt
  dw 0                         ; Limit (low).
  dw 0                         ; Base (low).
  db 0                         ; Base (middle)
  db 0b10011010                ; Access (exec/read).
  db 0b10101111                ; Granularity, 64 bits flag, limit19:16.
  db 0                         ; Base (high).

  GDT_DATA_SEGMENT equ $ - gdt
  dw 0                         ; Limit (low).
  dw 0                         ; Base (low).
  db 0                         ; Base (middle)
  db 0b10010010                ; Access (read/write).
  db 0b00000000                ; Granularity.
  db 0                         ; Base (high).

  GDT_USER_NULL_SEGMENT equ $ - gdt
  dw 0xFFFF                    ; Limit (low).
  dw 0                         ; Base (low).
  db 0                         ; Base (middle)
  db 0                         ; Access.
  db 1                         ; Granularity.
  db 0                         ; Base (high).

  GDT_USER_DATA_SEGMENT equ $ - gdt
  dw 0                         ; Limit (low).
  dw 0                         ; Base (low).
  db 0                         ; Base (middle)
  db 0b11110010                ; Access (read/write), ring 3
  db 0b00000000                ; Granularity.
  db 0                         ; Base (high).

  GDT_USER_CODE_SEGMENT equ $ - gdt
  dw 0                         ; Limit (low).
  dw 0                         ; Base (low).
  db 0                         ; Base (middle)
  db 0b11111010                ; Access (exec/read), ring 3
  db 0b10101111                ; Granularity, 64 bits flag, limit19:16.
  db 0                         ; Base (high).

  GDT_TASK_STATE_SEGMENT equ $ - gdt
  dw 0                         ; Limit (low).
  dw 0                         ; Base (low).
  db 0                         ; Base (middle)
  db 0x89
  db 0xA0
  db 0
  dq 0

gdtPointer:
  dw ($ - gdt - 1)               ; Limit
  dq (gdt - KERNEL_VIRTUAL_OFFSET)  ; Base

section .bss

align 4096

kernelPageTableL4:
  resq 512
global kernelPageTableL3
kernelPageTableL3:
  resq 512
kernelPageTableL2:
  resq 512

align 16
kernelStackBottom:
  resb (16 * 1024)
kernelStackTop:

global taskStateSegment
taskStateSegment:
  resb 13 * 8
  TSS_SIZE equ $ - taskStateSegment

section .text

bits 32
global _start
_start:
  cli

  ; Set up page mapping for temporary identity mapping (first 2 MB)
  ; and statically allocated kernel memory (-2 GB to -2 GB + 2 MB).
  mov dword [kernelPageTableL4 - KERNEL_VIRTUAL_OFFSET], (kernelPageTableL3 - KERNEL_VIRTUAL_OFFSET) + 0x03
  mov dword [kernelPageTableL4 - KERNEL_VIRTUAL_OFFSET + 511 * 8], (kernelPageTableL3 - KERNEL_VIRTUAL_OFFSET) + 0x03
  mov dword [kernelPageTableL3 - KERNEL_VIRTUAL_OFFSET], (kernelPageTableL2 - KERNEL_VIRTUAL_OFFSET) + 0x03
  mov dword [kernelPageTableL3 - KERNEL_VIRTUAL_OFFSET + 510 * 8], (kernelPageTableL2 - KERNEL_VIRTUAL_OFFSET) + 0x03
  mov dword [kernelPageTableL2 - KERNEL_VIRTUAL_OFFSET], 0x83

  mov edi, (kernelPageTableL4 - KERNEL_VIRTUAL_OFFSET)
  mov cr3, edi

  ; Enable PAE and PSE
  mov eax, cr4
  or eax, 0b0011_0000
  mov cr4, eax

  mov ecx, 0xC000_0080         ; Set the C-register to 0xC0000080, which is the EFER MSR.
  rdmsr                        ; Read from the model-specific register.
  or eax, 0x101                ; Set the LM-bit and system call extensions.
  wrmsr                        ; Write to the model-specific register.

  ; Enable paging
  mov eax, cr0
  or eax, 1 << 31
  mov cr0, eax

  lgdt [gdtPointer - KERNEL_VIRTUAL_OFFSET]
  jmp GDT_CODE_SEGMENT:(_start64_physical - KERNEL_VIRTUAL_OFFSET)

bits 64
_start64_physical:
  mov rax, _start64
  jmp rax
_start64:
  mov ax, GDT_DATA_SEGMENT
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  ; Filling TSS size and address in GDT
  mov word [gdt - KERNEL_VIRTUAL_OFFSET + GDT_TASK_STATE_SEGMENT], TSS_SIZE
  mov rax, taskStateSegment
  mov [gdt - KERNEL_VIRTUAL_OFFSET + GDT_TASK_STATE_SEGMENT + 2], ax
  shr rax, 16
  mov [gdt - KERNEL_VIRTUAL_OFFSET + GDT_TASK_STATE_SEGMENT + 4], al
  mov [gdt - KERNEL_VIRTUAL_OFFSET + GDT_TASK_STATE_SEGMENT + 7], ah
  shr rax, 16
  mov [gdt - KERNEL_VIRTUAL_OFFSET + GDT_TASK_STATE_SEGMENT + 8], eax

  ; Disable identity page
  mov qword [kernelPageTableL3], 0
  mov qword [kernelPageTableL4], 0
  ; Flush TLB
  mov rax, cr3
  mov cr3, rax

  ; Setting up registers for syscall & sysret
  mov ecx, STAR
  rdmsr
  mov dx, GDT_USER_NULL_SEGMENT
  shl edx, 16
  mov dx, GDT_CODE_SEGMENT
  wrmsr
  mov ecx, LSTAR
  extern syscall_entry
  mov rax, syscall_entry
  mov rdx, rax
  shr rdx, 32
  wrmsr

  mov qword [gdtPointer + 2], gdt
  mov rax, gdtPointer
  lgdt [rax]
  mov rsp, kernelStackTop

  mov ax, GDT_TASK_STATE_SEGMENT
  ltr ax

  extern idt_fill, idt_pointer
  call idt_fill
  mov rax, idt_pointer
  lidt [rax]

  ; Run constructors of statically allocated objects
  extern _init
  call _init

  ; The boot loader put the pointer to the multiboot2 information structure
  ; to rbx and we haven't changed it.
  mov rdi, KERNEL_VIRTUAL_OFFSET
  add rdi, rbx

  sti
  extern kernel_main
  call kernel_main
  ; TODO do we need this?
  extern _fini
  call _fini

global kernel_halt
kernel_halt:
  cli
.loop:
  hlt
  jmp .loop
