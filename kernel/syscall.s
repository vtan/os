.intel_syntax noprefix

.macro pushall
  push rbx
  push rbp
  push rdi
  push rsi
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
  pop rsi
  pop rdi
  pop rbp
  pop rbx
.endm

.global syscall_entry
syscall_entry:
  mov rdx, rsp
  movabs rax, [runningProcess]  # Switching to kernel stack
  mov rsp, [rax]
  push rdx  # RSP before syscall
  push r11  # RFLAGS before syscall
  push rcx  # RIP before syscall
  push rdi  # Syscall number
  push rsi  # Syscall argument
  pushall   # Callee-saved registers
  mov rdi, rsp
  jmp kernel_syscall
