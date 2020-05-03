.intel_syntax noprefix

.global Process_run
Process_run:
  mov rax, [rdi]
  movabs [tss + 4], rax    # Putting kernel stack pointer in TSS.RSP0

  mov rcx, [rdi + 8]     # Entry point
  mov rbp, [rdi + 16]    # User stack pointer

  mov rsp, rbp          # Setting up stack for process
  mov r11, 0x0202
  sysretq
