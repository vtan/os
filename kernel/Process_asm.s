.intel_syntax noprefix

.global Process_run
Process_run:
  # Currently the only syscall is done when exiting the user process,
  # we save the current stack pointer here and restore it in the syscall handler.
  push rbp
  mov rbx, rsp

  mov rcx, [rdi]        # Entry point
  mov rbp, [rdi + 8]    # User stack pointer
  mov rax, [rdi + 16]
  movabs [tss + 4], rax    # Putting kernel stack pointer in TSS.RSP0

  mov rsp, rbp          # Setting up stack for process
  mov r11, 0x0202
  sysretq
