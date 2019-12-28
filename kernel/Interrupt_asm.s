.intel_syntax noprefix

.macro pushall
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
.endm

.macro popall
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
.endm

.macro exception_stub n
interrupt_handler_\n:
  push 0 # Dummy error code
  push \n
  jmp generic_exception_handler
.endm

.macro exception_with_error_code_stub n
interrupt_handler_\n:
  push \n
  jmp generic_exception_handler
.endm

.macro irq_handler_stub n
interrupt_handler_\n:
  push 0 # Dummy error code
  push \n
  jmp generic_irq_handler
.endm

.macro idt_fill_record n
  movabs rax, offset interrupt_handler_\n

  mov word ptr [rdi + \n * 16 + 2], 0x08 # selector
  mov byte ptr [rdi + \n * 16 + 4], 0x00 # IST offset
  mov byte ptr [rdi + \n * 16 + 5], 0x8E # type
  mov dword ptr [rdi + \n * 16 + 12], 0x00 # unused

  mov word ptr [rdi + \n * 16 + 0], ax   # address bits 0-15
  shr rax, 16
  mov word ptr [rdi + \n * 16 + 6], ax   # address bits 16-31
  shr rax, 16
  mov dword ptr [rdi + \n * 16 + 8], eax   # address bits 32-63
.endm

.section .bss

.align 4096
idt:
  .skip (256 * 16)

.section .rodata

.global idt_pointer
idt_pointer:
  .2byte (256 * 16 - 1)
  .8byte offset idt

.section .text

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

generic_exception_handler:
  pushall
  mov rdi, rsp
  cld
  call Interrupt_genericExceptionHandler
  popall
  add rsp, 16
  jmp halt
  # iretq

generic_irq_handler:
  pushall
  mov rdi, rsp
  cld
  call Interrupt_genericIrqHandler
  popall
  add rsp, 16
  iretq

.global idt_fill
idt_fill:
  movabs rdi, offset idt
  idt_fill_record 0x00
  idt_fill_record 0x01
  idt_fill_record 0x02
  idt_fill_record 0x03
  idt_fill_record 0x04
  idt_fill_record 0x05
  idt_fill_record 0x06
  idt_fill_record 0x07
  idt_fill_record 0x08
  idt_fill_record 0x09
  idt_fill_record 0x0A
  idt_fill_record 0x0B
  idt_fill_record 0x0C
  idt_fill_record 0x0D
  idt_fill_record 0x0E
  idt_fill_record 0x0F
  idt_fill_record 0x10
  idt_fill_record 0x11
  idt_fill_record 0x12
  idt_fill_record 0x13
  idt_fill_record 0x14
  idt_fill_record 0x15
  idt_fill_record 0x16
  idt_fill_record 0x17
  idt_fill_record 0x18
  idt_fill_record 0x19
  idt_fill_record 0x1A
  idt_fill_record 0x1B
  idt_fill_record 0x1C
  idt_fill_record 0x1D
  idt_fill_record 0x1E
  idt_fill_record 0x1F
  idt_fill_record 0x20
  idt_fill_record 0x21
  idt_fill_record 0x22
  idt_fill_record 0x23
  idt_fill_record 0x24
  idt_fill_record 0x25
  idt_fill_record 0x26
  idt_fill_record 0x27
  idt_fill_record 0x28
  idt_fill_record 0x29
  idt_fill_record 0x2A
  idt_fill_record 0x2B
  idt_fill_record 0x2C
  idt_fill_record 0x2D
  idt_fill_record 0x2E
  idt_fill_record 0x2F
  ret
