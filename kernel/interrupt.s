bits 64

section .bss

align 4096
idt:
  resb (256 * 16)

section .rodata

global idt_pointer
idt_pointer:
  dw (256 * 16 - 1)
  dq idt

section .text

%macro pushall 0
  push rax
  push rcx
  push rdx
  push rbx
  push rbp
  push rsi
  push rdi
  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15
%endmacro

%macro popall 0
  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop rdi
  pop rsi
  pop rbp
  pop rbx
  pop rdx
  pop rcx
  pop rax
%endmacro

%macro exception_stub 1
interrupt_handler_%1:
  push 0    ; Dummy error code
  push %1
  jmp generic_exception_handler
%endmacro

%macro exception_with_error_code_stub 1
interrupt_handler_%1:
  push %1
  jmp generic_exception_handler
%endmacro

%macro irq_handler_stub 1
interrupt_handler_%1:
  push 0    ; Dummy error code
  push %1
  jmp generic_irq_handler
%endmacro

exception_stub 0x00
exception_stub 0x01
exception_stub 0x02
exception_stub 0x03
exception_stub 0x04
exception_stub 0x05
exception_stub 0x06
exception_stub 0x07
exception_with_error_code_stub 0x08
exception_stub 0x09
exception_with_error_code_stub 0x0A
exception_with_error_code_stub 0x0B
exception_with_error_code_stub 0x0C
exception_with_error_code_stub 0x0D
exception_with_error_code_stub 0x0E
exception_stub 0x0F
exception_stub 0x10
exception_with_error_code_stub 0x11
exception_stub 0x12
exception_stub 0x13
exception_stub 0x14
exception_stub 0x15
exception_stub 0x16
exception_stub 0x17
exception_stub 0x18
exception_stub 0x19
exception_stub 0x1A
exception_stub 0x1B
exception_stub 0x1C
exception_stub 0x1D
exception_with_error_code_stub 0x1E
exception_stub 0x1F
irq_handler_stub 0x20
irq_handler_stub 0x21
irq_handler_stub 0x22
irq_handler_stub 0x23
irq_handler_stub 0x24
irq_handler_stub 0x25
irq_handler_stub 0x26
irq_handler_stub 0x27
irq_handler_stub 0x28
irq_handler_stub 0x29
irq_handler_stub 0x2A
irq_handler_stub 0x2B
irq_handler_stub 0x2C
irq_handler_stub 0x2D
irq_handler_stub 0x2E
irq_handler_stub 0x2F

extern kernel_exception, kernel_halt, kernel_irq
generic_exception_handler:
  pushall
  mov rdi, rsp
  cld
  call kernel_exception
  popall
  add rsp, 16
  jmp kernel_halt

generic_irq_handler:
  pushall
  mov rdi, rsp
  cld
  call kernel_irq
  popall
  add rsp, 16
  iretq

%macro call_idt_fill_record 1
  mov rax, interrupt_handler_%1
  call idt_fill_record
  add rdi, 16
%endmacro

global idt_fill
idt_fill:
  mov rdi, idt
  call_idt_fill_record 0x00
  call_idt_fill_record 0x01
  call_idt_fill_record 0x02
  call_idt_fill_record 0x03
  call_idt_fill_record 0x04
  call_idt_fill_record 0x05
  call_idt_fill_record 0x06
  call_idt_fill_record 0x07
  call_idt_fill_record 0x08
  call_idt_fill_record 0x09
  call_idt_fill_record 0x0A
  call_idt_fill_record 0x0B
  call_idt_fill_record 0x0C
  call_idt_fill_record 0x0D
  call_idt_fill_record 0x0E
  call_idt_fill_record 0x0F
  call_idt_fill_record 0x10
  call_idt_fill_record 0x11
  call_idt_fill_record 0x12
  call_idt_fill_record 0x13
  call_idt_fill_record 0x14
  call_idt_fill_record 0x15
  call_idt_fill_record 0x16
  call_idt_fill_record 0x17
  call_idt_fill_record 0x18
  call_idt_fill_record 0x19
  call_idt_fill_record 0x1A
  call_idt_fill_record 0x1B
  call_idt_fill_record 0x1C
  call_idt_fill_record 0x1D
  call_idt_fill_record 0x1E
  call_idt_fill_record 0x1F
  call_idt_fill_record 0x20
  call_idt_fill_record 0x21
  call_idt_fill_record 0x22
  call_idt_fill_record 0x23
  call_idt_fill_record 0x24
  call_idt_fill_record 0x25
  call_idt_fill_record 0x26
  call_idt_fill_record 0x27
  call_idt_fill_record 0x28
  call_idt_fill_record 0x29
  call_idt_fill_record 0x2A
  call_idt_fill_record 0x2B
  call_idt_fill_record 0x2C
  call_idt_fill_record 0x2D
  call_idt_fill_record 0x2E
  call_idt_fill_record 0x2F
  ret

idt_fill_record:
  mov word [rdi + 2], 0x08    ; selector
  mov byte [rdi + 4], 0x00    ; IST offset
  mov byte [rdi + 5], 0x8E    ; type
  mov dword [rdi + 12], 0x00  ; unused

  mov word [rdi + 0], ax   ; address bits 0-15
  shr rax, 16
  mov word [rdi + 6], ax   ; address bits 16-31
  shr rax, 16
  mov dword [rdi + 8], eax   ; address bits 32-63
  ret
