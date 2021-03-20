section .text

global _start
_start:
  extern main
  call main
  mov rdi, 0
  mov rsi, rax
  syscall

global syscall
syscall:
  syscall
  ret
