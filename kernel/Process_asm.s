.intel_syntax noprefix

.global Process_run
Process_run:
  push rbp
  mov rcx, [rdi]        # Entry point
  mov rbp, [rdi + 8]    # Top of stack
  mov rbx, rsp          # Saving current stack pointer to callee-saved register
  mov rsp, rbp          # Setting up stack for process
  call rcx
  mov rsp, rbx
  pop rbp
  ret
