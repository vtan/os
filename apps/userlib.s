.intel_syntax noprefix

.global _start
_start:
  call main
  mov rdi, 0
  mov rsi, rax
  syscall

.global syscall
syscall:
  syscall
  ret
